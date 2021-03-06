//  Copyright (c) 2020 ETH Zurich
//
//  SPDX-License-Identifier: BSL-1.0
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <hpx/local/config.hpp>
#include <hpx/concepts/concepts.hpp>
#include <hpx/datastructures/optional.hpp>
#include <hpx/datastructures/variant.hpp>
#include <hpx/execution/algorithms/detail/single_result.hpp>
#include <hpx/execution_base/operation_state.hpp>
#include <hpx/execution_base/receiver.hpp>
#include <hpx/execution_base/sender.hpp>
#include <hpx/functional/detail/tag_fallback_invoke.hpp>
#include <hpx/functional/invoke_fused.hpp>
#include <hpx/type_support/pack.hpp>

#include <atomic>
#include <cstddef>
#include <exception>
#include <functional>
#include <memory>
#include <type_traits>
#include <utility>

namespace hpx { namespace execution { namespace experimental {
    namespace detail {
        // This is a receiver to be connected to the ith predecessor sender
        // passed to when_all. When set_value is called, it will emplace the
        // values sent into the appropriate position in the pack used to store
        // values from all predecessor senders.
        template <typename OperationState>
        struct when_all_receiver
        {
            std::decay_t<OperationState>& op_state;

            when_all_receiver(std::decay_t<OperationState>& op_state)
              : op_state(op_state)
            {
            }

            template <typename Error>
            friend void tag_invoke(
                set_error_t, when_all_receiver&& r, Error&& error) noexcept
            {
                if (!r.op_state.set_done_error_called.exchange(true))
                {
                    try
                    {
                        r.op_state.error = HPX_FORWARD(Error, error);
                    }
                    catch (...)
                    {
                        // NOLINTNEXTLINE(bugprone-throw-keyword-missing)
                        r.op_state.error = std::current_exception();
                    }
                }

                r.op_state.finish();
            }

            friend void tag_invoke(set_done_t, when_all_receiver&& r) noexcept
            {
                r.op_state.set_done_error_called = true;
                r.op_state.finish();
            };

            template <typename... Ts, std::size_t... Is>
            auto set_value_helper(hpx::util::index_pack<Is...>, Ts&&... ts)
                -> decltype((
                    std::declval<
                        typename OperationState::value_types_storage_type>()
                        .template get<OperationState::i_storage_offset + Is>()
                        .emplace(HPX_FORWARD(Ts, ts)),
                    ...))
            {
                // op_state.ts holds values from all predecessor senders. We
                // emplace the values using the offset calculated while
                // constructing the operation state.
                (op_state.ts
                        .template get<OperationState::i_storage_offset + Is>()
                        .emplace(HPX_FORWARD(Ts, ts)),
                    ...);
            }

            using index_pack_type = typename hpx::util::make_index_pack<
                OperationState::sender_pack_size>::type;

            template <typename... Ts>
            auto set_value(Ts&&... ts) noexcept -> decltype(
                set_value_helper(index_pack_type{}, HPX_FORWARD(Ts, ts)...))
            {
                if constexpr (OperationState::sender_pack_size > 0)
                {
                    if (!op_state.set_done_error_called)
                    {
                        try
                        {
                            set_value_helper(
                                index_pack_type{}, HPX_FORWARD(Ts, ts)...);
                        }
                        catch (...)
                        {
                            if (!op_state.set_done_error_called.exchange(true))
                            {
                                // NOLINTNEXTLINE(bugprone-throw-keyword-missing)
                                op_state.error = std::current_exception();
                            }
                        }
                    }
                }

                op_state.finish();
            }
        };

        // Due to what appears to be a bug in clang this is not a hidden friend
        // of when_all_receiver. The trailing decltype for SFINAE in the member
        // set_value would give an error about accessing an incomplete type, if
        // the member set_value were a hidden friend tag_invoke overload
        // instead.
        template <typename OperationState, typename... Ts>
        auto tag_invoke(set_value_t, when_all_receiver<OperationState>&& r,
            Ts&&... ts) noexcept
            -> decltype(r.set_value(HPX_FORWARD(Ts, ts)...))
        {
            r.set_value(HPX_FORWARD(Ts, ts)...);
        }

        template <typename... Senders>
        struct when_all_sender
        {
            using senders_type =
                hpx::util::member_pack_for<std::decay_t<Senders>...>;
            senders_type senders;

            template <typename... Senders_>
            explicit constexpr when_all_sender(Senders_&&... senders)
              : senders(
                    std::piecewise_construct, HPX_FORWARD(Senders_, senders)...)
            {
            }

            template <typename Sender>
            struct value_types_helper
            {
                using value_types =
                    typename hpx::execution::experimental::sender_traits<
                        Sender>::template value_types<hpx::util::pack,
                        hpx::util::pack>;
                using type = detail::single_result_non_void_t<value_types>;
            };

            template <typename Sender>
            using value_types_helper_t =
                typename value_types_helper<Sender>::type;

            template <template <typename...> class Tuple,
                template <typename...> class Variant>
            using value_types = hpx::util::detail::concat_inner_packs_t<
                hpx::util::detail::concat_t<
                    typename hpx::execution::experimental::sender_traits<
                        Senders>::template value_types<Tuple, Variant>...>>;

            template <template <typename...> class Variant>
            using error_types = hpx::util::detail::unique_concat_t<
                typename hpx::execution::experimental::sender_traits<
                    Senders>::template error_types<Variant>...,
                Variant<std::exception_ptr>>;

            static constexpr bool sends_done = false;

            static constexpr std::size_t num_predecessors = sizeof...(Senders);
            static_assert(num_predecessors > 0,
                "when_all expects at least one predecessor sender");

            template <std::size_t I>
            static constexpr std::size_t sender_pack_size_at_index =
                single_variant_t<typename sender_traits<hpx::util::at_index_t<I,
                    Senders...>>::template value_types<hpx::util::pack,
                    hpx::util::pack>>::size;

            template <typename Receiver, typename SendersPack,
                std::size_t I = num_predecessors - 1>
            struct operation_state;

            template <typename Receiver, typename SendersPack>
            struct operation_state<Receiver, SendersPack, 0>
            {
                // The index of the sender that this operation state handles.
                static constexpr std::size_t i = 0;
                // The offset at which we start to emplace values sent by the
                // ith predecessor sender.
                static constexpr std::size_t i_storage_offset = 0;
#if !defined(HPX_CUDA_VERSION)
                // The number of values sent by the ith predecessor sender.
                static constexpr std::size_t sender_pack_size =
                    sender_pack_size_at_index<i>;
#else
                // nvcc does not like using the helper sender_pack_size_at_index
                // here and complains about incmplete types. Lifting the helper
                // explicitly in here works.
                static constexpr std::size_t sender_pack_size =
                    single_variant_t<
                        typename sender_traits<hpx::util::at_index_t<i,
                            Senders...>>::template value_types<hpx::util::pack,
                            hpx::util::pack>>::size;
#endif

                // Number of predecessor senders that have not yet called any of
                // the set signals.
                std::atomic<std::size_t> predecessors_remaining =
                    num_predecessors;

                template <typename T>
                struct add_optional
                {
                    using type = hpx::optional<std::decay_t<T>>;
                };
                using value_types_storage_type =
                    hpx::util::detail::change_pack_t<hpx::util::member_pack_for,
                        hpx::util::detail::transform_t<
                            hpx::util::detail::concat_pack_of_packs_t<
                                value_types<hpx::util::pack, hpx::util::pack>>,
                            add_optional>>;
                // Values sent by all predecessor senders are stored here in the
                // base-case operation state. They are stored in a
                // member_pack<optional<T0>, ..., optional<Tn>>, where T0, ...,
                // Tn are the types of the values sent by all predecessor
                // senders.
                value_types_storage_type ts;

                hpx::optional<error_types<hpx::variant>> error;
                std::atomic<bool> set_done_error_called{false};
                HPX_NO_UNIQUE_ADDRESS std::decay_t<Receiver> receiver;

                using operation_state_type =
                    std::decay_t<decltype(hpx::execution::experimental::connect(
                        std::declval<SendersPack>().template get<i>(),
                        when_all_receiver<operation_state>(
                            std::declval<std::decay_t<operation_state>&>())))>;
                operation_state_type op_state;

                template <typename Receiver_, typename Senders_>
                operation_state(Receiver_&& receiver, Senders_&& senders)
                  : receiver(HPX_FORWARD(Receiver_, receiver))
                  , op_state(hpx::execution::experimental::connect(
#if defined(HPX_CUDA_VERSION)
                        std::forward<Senders_>(senders).template get<i>(),
#else
                        HPX_FORWARD(Senders_, senders).template get<i>(),
#endif
                        when_all_receiver<operation_state>(*this)))
                {
                }

                operation_state(operation_state&&) = delete;
                operation_state& operator=(operation_state&&) = delete;
                operation_state(operation_state const&) = delete;
                operation_state& operator=(operation_state const&) = delete;

                void start() & noexcept
                {
                    hpx::execution::experimental::start(op_state);
                }

                template <std::size_t... Is, typename... Ts>
                void set_value_helper(
                    hpx::util::member_pack<hpx::util::index_pack<Is...>, Ts...>&
                        ts)
                {
                    hpx::execution::experimental::set_value(HPX_MOVE(receiver),
                        HPX_MOVE(*(ts.template get<Is>()))...);
                }

                void finish() noexcept
                {
                    if (--predecessors_remaining == 0)
                    {
                        if (!set_done_error_called)
                        {
                            set_value_helper(ts);
                        }
                        else if (error)
                        {
                            hpx::visit(
                                [this](auto&& error) {
                                    hpx::execution::experimental::set_error(
                                        HPX_MOVE(receiver),
                                        HPX_FORWARD(decltype(error), error));
                                },
                                HPX_MOVE(error.value()));
                        }
                        else
                        {
                            hpx::execution::experimental::set_done(
                                HPX_MOVE(receiver));
                        }
                    }
                }
            };

            template <typename Receiver, typename SendersPack, std::size_t I>
            struct operation_state
              : operation_state<Receiver, SendersPack, I - 1>
            {
                using base_type = operation_state<Receiver, SendersPack, I - 1>;

                // The index of the sender that this operation state handles.
                static constexpr std::size_t i = I;
                // The number of values sent by the ith predecessor sender.
                static constexpr std::size_t sender_pack_size =
                    sender_pack_size_at_index<i>;
                // The offset at which we start to emplace values sent by the
                // ith predecessor sender.
                static constexpr std::size_t i_storage_offset =
                    base_type::i_storage_offset + base_type::sender_pack_size;

                using operation_state_type =
                    std::decay_t<decltype(hpx::execution::experimental::connect(
                        HPX_FORWARD(SendersPack, senders).template get<i>(),
                        when_all_receiver<operation_state>(
                            std::declval<std::decay_t<operation_state>&>())))>;
                operation_state_type op_state;

                template <typename Receiver_, typename SendersPack_>
                operation_state(Receiver_&& receiver, SendersPack_&& senders)
                  : base_type(HPX_FORWARD(Receiver_, receiver),
                        HPX_FORWARD(SendersPack, senders))
                  , op_state(hpx::execution::experimental::connect(
#if defined(HPX_CUDA_VERSION)
                        std::forward<SendersPack_>(senders).template get<i>(),
#else
                        HPX_FORWARD(SendersPack_, senders).template get<i>(),
#endif
                        when_all_receiver<operation_state>(*this)))
                {
                }

                operation_state(operation_state&&) = delete;
                operation_state& operator=(operation_state&&) = delete;
                operation_state(operation_state const&) = delete;
                operation_state& operator=(operation_state const&) = delete;

                void start() & noexcept
                {
                    base_type::start();
                    hpx::execution::experimental::start(op_state);
                }
            };

            template <typename Receiver, typename SendersPack>
            friend void tag_invoke(start_t,
                operation_state<Receiver, SendersPack, num_predecessors - 1>&
                    os) noexcept
            {
                os.start();
            }

            template <typename Receiver>
            friend auto tag_invoke(
                connect_t, when_all_sender&& s, Receiver&& receiver)
            {
                return operation_state<Receiver, senders_type&&,
                    num_predecessors - 1>(
                    HPX_FORWARD(Receiver, receiver), HPX_MOVE(s.senders));
            }

            template <typename Receiver>
            friend auto tag_invoke(
                connect_t, when_all_sender& s, Receiver&& receiver)
            {
                return operation_state<Receiver, senders_type&,
                    num_predecessors - 1>(receiver, s.senders);
            }
        };
    }    // namespace detail

    inline constexpr struct when_all_t final
      : hpx::functional::detail::tag_fallback<when_all_t>
    {
    private:
        // clang-format off
        template <typename... Senders,
            HPX_CONCEPT_REQUIRES_(
                hpx::util::all_of_v<is_sender<Senders>...>
            )>
        // clang-format on
        friend constexpr HPX_FORCEINLINE auto tag_fallback_invoke(
            when_all_t, Senders&&... senders)
        {
            return detail::when_all_sender<Senders...>{
                HPX_FORWARD(Senders, senders)...};
        }
    } when_all{};
}}}    // namespace hpx::execution::experimental
