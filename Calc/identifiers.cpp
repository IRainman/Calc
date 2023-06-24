// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

/*
 * Copyright 2023 Solomina Elle Leonovna, a.rainman on gmail point com
 */

#include "pch.h"
#include "identifiers.h"

namespace
{
	using Value = Identifiers::Value;
	using Fn = Identifiers::Fn;

	template <const size_t>
	using WrappedFnImplArg = Value;

	template <const size_t... Is>
	consteval auto WrappedFnImpl(std::index_sequence<Is...>) noexcept -> Value(*)(WrappedFnImplArg<Is>...);

	template <const size_t N>
	using WrappedFn = decltype(WrappedFnImpl(std::make_index_sequence<N>()));

	template <typename Fn, const size_t... Is>
	[[nodiscard]] constexpr auto call_fn(Fn fn, std::span<const Value> params, std::index_sequence<Is...>) noexcept
	{
		return fn(params[Is]...);
	}

	template<const size_t N, WrappedFn<N> wrappedFn>
	[[nodiscard]] constexpr auto function_pointer_impl(std::span<const Value> params) noexcept
	{
		return call_fn(wrappedFn, params, std::make_index_sequence<N>());
	}

	template<const size_t N, WrappedFn<N> wrappedFn>
	[[nodiscard]] consteval Fn function_pointer() noexcept
	{
		return { { N, 0 }, function_pointer_impl<N, wrappedFn> };
	}

	template <const Value value>
	[[nodiscard]] constexpr auto constant_impl(std::span<const Value>) noexcept
	{
		return value;
	}

	template <const Value value>
	[[nodiscard]] consteval Fn constant() noexcept
	{
		return { { 0, 0 }, constant_impl<value> };
	}

	[[nodiscard]] Value power(std::span<const Value> params) noexcept
	{
		if (std::fabs(params[0] - 2) < std::numeric_limits<Value>::epsilon())
		{
			return std::exp2(params[1]);
		}
		else
		{
			return std::pow(params[0], params[1]);
		}
	}

	[[nodiscard]] constexpr Value minimum(std::span<const Value> params) noexcept
	{
		return std::ranges::min(params);
	}

	[[nodiscard]] constexpr Value maximum(std::span<const Value> params) noexcept
	{
		return std::ranges::max(params);
	}

	[[nodiscard]] constexpr Value rad(const Value x) noexcept { return x * std::numbers::pi_v<Value> / 180; }
	[[nodiscard]] constexpr Value deg(const Value x) noexcept { return x * 180 / std::numbers::pi_v<Value>; }

	[[nodiscard]] constexpr Value hypotenuse(std::span<const Value> params) noexcept
	{
		switch (params.size())
		{
		case 2:
			return std::hypot(params[0], params[1]);
		case 3:
			return std::hypot(params[0], params[1], params[2]);
		};
		std::unreachable();
	}

	[[nodiscard]] constexpr Value logarithm(Value x) noexcept
	{
		if (1 < x && x < 2)
		{
			return std::log1p(--x);
		}
		else
		{
			return std::log(x);
		}
	}

	[[nodiscard]] Value assoc_legendre(std::span<const Value> params) noexcept
	{
		return std::assoc_legendre(static_cast<const unsigned int>(params[0]), static_cast<const unsigned int>(params[1]), params[2]); //-V2004
	}

	[[nodiscard]] Value assoc_laguerre(std::span<const Value> params) noexcept
	{
		return std::assoc_laguerre(static_cast<const unsigned int>(params[0]), static_cast<const unsigned int>(params[1]), params[2]); //-V2004
	}

	[[nodiscard]] Value hermite(std::span<const Value> params) noexcept
	{
		return std::hermite(static_cast<const unsigned int>(params[0]), params[1]); //-V2004
	}

	[[nodiscard]] Value legendre(std::span<const Value> params) noexcept
	{
		return std::legendre(static_cast<const unsigned int>(params[0]), params[1]); //-V2004
	}

	[[nodiscard]] Value laguerre(std::span<const Value> params) noexcept
	{
		return std::laguerre(static_cast<const unsigned int>(params[0]), params[1]); //-V2004
	}

	[[nodiscard]] Value sph_bessel(std::span<const Value> params) noexcept
	{
		return std::sph_bessel(static_cast<const unsigned int>(params[0]), params[1]); //-V2004
	}
	[[nodiscard]] Value sph_legendre(std::span<const Value> params) noexcept
	{
		return std::sph_legendre(static_cast<const unsigned int>(params[0]), static_cast<const unsigned int>(params[1]), params[2]); //-V2004
	}
	[[nodiscard]] Value sph_neumann(std::span<const Value> params) noexcept
	{
		return std::sph_neumann(static_cast<const unsigned int>(params[0]), params[1]); //-V2004
	}

	static const Identifiers::map ids =
	{
		//---------------------------------------------------------------------------
		// TODO https://en.cppreference.com/w/cpp/numeric/constants
		{"pi", constant<std::numbers::pi>()},
		{"e", constant<std::numbers::e>()},
		{"phi", constant<std::numbers::phi>()},
		{"egamma", constant<std::numbers::egamma>()},
		//---------------------------------------------------------------------------
		// TODO additional constant
		{"c", constant<299792458.0L>()},// Speed of light in vacuum (m·s-1)
		{"G", constant<6.674286767676767676767676767676767676e-11L>()},// Newtonian constant of gravitation (m3·kg−1·s−2), this constant valid on Earths only ;)
		{"J", constant<3.058198247456354132564564787888767L>()},// Constants of Gauss fild
		{"atm", constant<101325.0L>()},// Standard atmosphere (Pa), this constant valid on Earths only ;)
		{"L", constant<6.022141510101010101010101010101010101e23L>()},// Avogadro's number (mol−1)
		{"R", constant<8.314472151515151515151515151515151515L>()},// Gas constant (J·K−1·mol−1)
		{"h", constant<6.626068963333333333333333333333333333e-34L>()},// Planck constant (J·s)
		//---------------------------------------------------------------------------
		// TODO https://en.cppreference.com/w/cpp/numeric/math
		{"sin", function_pointer<1, std::sin>()},
		{"cos", function_pointer<1, std::cos>()},
		{"tan", function_pointer<1, std::tan>()},
		{"arccos", function_pointer<1, std::acos>()},
		{"arcsin", function_pointer<1, std::asin>()},
		{"arctan", function_pointer<1, std::atan>()},

		{"hypot", {{2, 3}, hypotenuse}},

		{"exp", function_pointer<1, std::exp>()},

		{"sh", function_pointer<1, std::sinh>()},
		{"ch", function_pointer<1, std::cosh>()},
		{"tanh", function_pointer<1, std::tanh>()},
		{"asinh", function_pointer<1, std::asinh>()},
		{"acosh", function_pointer<1, std::acosh>()},
		{"atanh", function_pointer<1, std::atanh>()},

		{"ln", function_pointer<1, logarithm>()},
		{"log", function_pointer<1, logarithm>()},

		{"lg", function_pointer<1, std::log10>()},
		{"log10", function_pointer<1, std::log10>()},

		{"lb", function_pointer<1, std::log2>()},
		{"log2", function_pointer<1, std::log2>()},

		{"sqrt", function_pointer<1, std::sqrt>()},
		{"cbrt", function_pointer<1, std::cbrt>()},
		{"pow", {{2, 0}, power}},

		{"fma", function_pointer<3, std::fma>()},

		{"rad", function_pointer<1, rad>()},
		{"deg", function_pointer<1, deg>()},

		{"min", {{1, Fn::P::unlim}, minimum}},
		{"max", {{1, Fn::P::unlim}, maximum}},

		{"abs", function_pointer<1, std::abs>()},

		{"erf", function_pointer<1, std::erf>()},
		{"erfc", function_pointer<1, std::erfc>()},

		{"tgamma", function_pointer<1, tgamma>()},
		{"lgamma", function_pointer<1, lgamma>()},

		{"trunc", function_pointer<1, trunc>()},
		{"round", function_pointer<1, round>()},

		//---------------------------------------------------------------------------
		// TODO https://en.cppreference.com/w/cpp/numeric/special_math
		{"assoc_legendre", {{3, 0}, assoc_legendre}},
		{"assoc_laguerre", {{3, 0}, assoc_laguerre}},

		{"legendre", {{2, 0}, legendre}},
		{"laguerre", {{2, 0}, laguerre}},

		{"beta", function_pointer<2, std::beta>()},

		{"comp_ellint_1", function_pointer<1, std::comp_ellint_1>()},
		{"comp_ellint_2", function_pointer<1, std::comp_ellint_2>()},
		{"comp_ellint_3", function_pointer<2, std::comp_ellint_3>()},

		{"cyl_bessel_i", function_pointer<2, std::cyl_bessel_i>()},
		{"cyl_bessel_k", function_pointer<2, std::cyl_bessel_k>()},
		{"cyl_neumann", function_pointer<2, std::cyl_neumann>()},

		{"sph_bessel", {{2, 0}, sph_bessel}},
		{"sph_legendre", {{3, 0}, sph_legendre}},
		{"sph_neumann", {{2, 0}, sph_neumann}},

		{"ellint_1", function_pointer<2, std::ellint_1>()},
		{"ellint_2", function_pointer<2, std::ellint_2>()},
		{"ellint_3", function_pointer<3, std::ellint_3>()},

		{"expint", function_pointer<1, std::expint>()},

		{"hermite", {{2, 0}, hermite}},

		{"riemann_zeta", function_pointer<1, std::riemann_zeta>()},
		//---------------------------------------------------------------------------
	};
}

const Identifiers::map& Identifiers::get() noexcept
{
	return ids;
}