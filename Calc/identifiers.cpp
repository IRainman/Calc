// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

/*
 * Copyright 2023-2025 Solomina Elle, a.rainman on gmail point com
 */

#include "pch.h"
#include "identifiers.h"

namespace Identifiers
{
	template <const ParamCount>
	using WrappedFnImplArg = Value;

	template<const ParamCount ...Is>
	consteval auto WrappedFnImpl(std::index_sequence<Is...>) noexcept -> Value(*)(WrappedFnImplArg<Is>...)
	{
		// This function only returns a type, so the body can be empty.
		return nullptr;
	}

	template <const ParamCount N>
	using WrappedFn = decltype(WrappedFnImpl(std::make_index_sequence<N>()));

	template <typename Fn, const ParamCount... Is>
	[[nodiscard]] constexpr/*consteval*/ auto call_fn(Fn fn, std::span<const Value> params, std::index_sequence<Is...>) noexcept
	{
		return fn(params[Is]...);
	}

	template<const ParamCount N, WrappedFn<N> wrappedFn>
	[[nodiscard]] constexpr/*consteval*/ auto function_pointer_impl(std::span<Value> params) noexcept
	{
		return call_fn(wrappedFn, params, std::make_index_sequence<N>());
	}

	template<const ParamCount N, WrappedFn<N> wrappedFn>
	[[nodiscard]] consteval Fn function_pointer() noexcept
	{
		return { { N, N }, function_pointer_impl<N, wrappedFn> };
	}

	template <Value value>
	[[nodiscard]] constexpr/*consteval*/ auto constant_impl(std::span<Value>) noexcept
	{
		return value;
	}

	template <Value value>
	[[nodiscard]] consteval Fn constant() noexcept
	{
		return { { 0, 0 }, constant_impl<value> };
	}

	[[nodiscard]] constexpr Value min(std::span<Value> params) noexcept
	{
		return std::ranges::min(params);
	}

	[[nodiscard]] constexpr Value max(std::span<Value> params) noexcept
	{
		return std::ranges::max(params);
	}

	[[nodiscard]] /*constexpr*/ bool compare(const Value a, const Value b) noexcept
	{
		return std::abs(a - b) <= std::numeric_limits<Value>::epsilon() * std::max(std::abs(a), std::abs(b));
	}

	[[nodiscard]] constexpr Value rad(Value x) noexcept { return x * std::numbers::pi_v<Value> / 180.0; }
	[[nodiscard]] constexpr Value deg(Value x) noexcept { return x * 180.0 / std::numbers::pi_v<Value>; }

	[[nodiscard]] constexpr Value hypot(std::span<Value> params) noexcept
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
#ifdef CALC_TESTS_ENABLED
	[[nodiscard]] constexpr Value parabola(std::span<Value> params) noexcept
	{
		return params[0] * params[0] + 1.0;
	}
#endif
	[[nodiscard]] /*constexpr*/ Value pow(Value x, Value y) noexcept
	{
		if (compare(x, std::numbers::e_v<Value>))
		{
			// for better precision
			return std::exp(y);
		}
		if (compare(y, 1.0 / 2.0))
		{
			// for better precision
			return std::sqrt(x);
		}
		if (compare(y, 1.0 / 3.0))
		{
			// for better precision
			return std::cbrt(x);
		}
		return std::pow(x, y);
	}

	[[nodiscard]] /*constexpr*/static Value assoc_legendre(std::span<Value> params) noexcept
	{
		return std::assoc_legendre(static_cast<unsigned int>(params[0]), static_cast<unsigned int>(params[1]), params[2]); //-V2004
	}

	[[nodiscard]] /*constexpr*/static Value assoc_laguerre(std::span<Value> params) noexcept
	{
		return std::assoc_laguerre(static_cast<unsigned int>(params[0]), static_cast<unsigned int>(params[1]), params[2]); //-V2004
	}

	[[nodiscard]] /*constexpr*/static Value hermite(std::span<Value> params) noexcept
	{
		return std::hermite(static_cast<unsigned int>(params[0]), params[1]); //-V2004
	}

	[[nodiscard]] /*constexpr*/static Value legendre(std::span<Value> params) noexcept
	{
		return std::legendre(static_cast<unsigned int>(params[0]), params[1]); //-V2004
	}

	[[nodiscard]] /*constexpr*/static Value laguerre(std::span<Value> params) noexcept
	{
		return std::laguerre(static_cast<unsigned int>(params[0]), params[1]); //-V2004
	}

	[[nodiscard]] /*constexpr*/static Value sph_bessel(std::span<Value> params) noexcept
	{
		return std::sph_bessel(static_cast<unsigned int>(params[0]), params[1]); //-V2004
	}

	[[nodiscard]] /*constexpr*/static Value sph_legendre(std::span<Value> params) noexcept
	{
		return std::sph_legendre(static_cast<unsigned int>(params[0]), static_cast<unsigned int>(params[1]), params[2]); //-V2004
	}

	[[nodiscard]] /*constexpr*/static Value sph_neumann(std::span<Value> params) noexcept
	{
		return std::sph_neumann(static_cast<unsigned int>(params[0]), params[1]); //-V2004
	}

#ifdef CALC_TESTS_ENABLED
	[[nodiscard]] /*constexpr*/static Value distance(std::span<Value> params) noexcept
	{
		// Minkowski distance: parameters contains p all other parameters is distances d[i] = v[i] - w[i] for two vectors v and w
		Value ex = 0.0;
		Value min_d = std::numeric_limits<Value>::infinity();
		Value max_d = -std::numeric_limits<Value>::infinity();
		for (size_t i = 1; i != params.size(); ++i)
		{
			params[i] = std::abs(params[i]);
			ex += pow(params[i], params[0]);
			min_d = std::min(min_d, params[i]);
			max_d = std::max(max_d, params[i]);
		}

		return std::isnan(ex) ? ex
			: !std::isnormal(ex) && std::signbit(params[0]) ? min_d
			: !std::isnormal(ex) && !std::signbit(params[0]) ? max_d
			: pow(ex, 1.0 / params[0]);
	}
#endif

	static const map ids =
	{
		// https://en.cppreference.com/w/cpp/numeric/constants 
		{ "pi", constant<std::numbers::pi_v<Value>>() }, // https://en.wikipedia.org/wiki/Pi_(mathematical_constant)
		{ "e", constant<std::numbers::e_v<Value>>() }, // https://en.wikipedia.org/wiki/E_(mathematical_constant)
		{ "phi", constant<std::numbers::phi_v<Value>>() }, // https://en.wikipedia.org/wiki/Golden_ratio
		{ "egamma", constant<std::numbers::egamma_v<Value>>() }, // https://en.wikipedia.org/wiki/Euler%27s_constant
#ifdef CALC_TESTS_ENABLED
		{ "log2e", constant<std::numbers::log2e_v<Value>>() },
		{ "log10e", constant<std::numbers::log10e_v<Value>>() },
		{ "inv_pi", constant<std::numbers::inv_pi_v<Value>>() },
		{ "inv_sqrtpi", constant<std::numbers::inv_sqrtpi_v<Value>>() },
		{ "ln2", constant<std::numbers::ln2_v<Value>>() },
		{ "ln10", constant<std::numbers::ln10_v<Value>>() },
		{ "sqrt2", constant<std::numbers::sqrt2_v<Value>>() },
		{ "sqrt3", constant<std::numbers::sqrt3_v<Value>>() },
		{ "inv_sqrt3", constant<std::numbers::inv_sqrt3_v<Value>>() },
#endif
		{ "inf", constant<std::numeric_limits<Value>::infinity()>() },
		//---------------------------------------------------------------------------
		// TODO additional constant
		{ "c", constant<299792458.0>() }, // Speed of light in vacuum (m*s^-1)
		{ "G", constant<6.6743015151515151515151515151515151e-11>() }, // Newtonian constant of gravitation (m^3*kg^−1*s^−2)
		{ "J", constant<3.058198247456354132564564787888767>() }, // Constants of Gauss field
		{ "atm", constant<101325.0>() }, // Standard atmosphere (Pa)
		{ "g_n", constant<9.80665>() }, // Standard acceleration of gravity (m*s^-2)
		{ "N_A", constant<6.02214076e23>() }, // Avogadro's number (mol^−1)
		{ "k", constant<1.380649e-23>() }, // Boltzmann constant (J*K−1)
		{ "F", constant<9.64853321233100184e4>() }, // Faraday constant (C*mol^−1)
		{ "R", constant<8.31446261815324>() }, // Molar gas constant (J*K^−1*mol^−1)
		{ "h", constant<6.62607015e-34>() }, // Planck constant (J*s)
		{ "G_0", constant<7.748091729729729729729729729729729729729729729729729e-5>() }, // Conductance quantum (S)
		{ "l_P", constant<1.616255181818181818181818181818181818181818e-35>() }, // Planck length (m)
		{ "m_P", constant<2.176434242424242424242424242424242424242424e-8>() }, // Planck mass (kg)
		{ "T_P", constant<1.4167841616161616161616161616161616161616161616161616e32>() }, // Planck temperature (K)
		{ "t_P", constant<5.391247606060606060606060606060606060606060606060e-44>() }, // Planck time (s)
		{ "mu_0", constant<1.2566370621219191919191919191919191919e-6>() }, // magnetic constant (exactly 4 pi * 10^-7)
		{ "eps_0", constant<8.854187817620389850536563031710750260608e-12>() }, // electric constant (Ohm) (F*m^-1)
		{ "Z_0", constant<376.7303134617706554681984004203193082686>() }, // characteristic impedance of vacuum (Ohm)
		{ "e_0", constant<1.602176634e-19>() }, // Elementary charge (C)
		{ "eV",  constant<1.602176634e-19>() }, // Electronvolt (J)
		{ "m_e", constant<9.1093837139282828282828282828282828282828282828282828282828282828e-31>() }, // Electron mass (kg)
		{ "m_p", constant<1.67262192595525252525252525252525252525252525252525252525252525252e-27>() }, // Proton mass (kg)
		{ "m_n", constant<1.67492750056858585858585858585858585858585858585858585858585858585e-27>() }, // Neutron mass (kg)
		{ "m_u", constant<1.66053906892e-27>() }, // Atomic mass constant (kg)

		//---------------------------------------------------------------------------
		// https://en.cppreference.com/w/cpp/numeric/math
		{ "sin", function_pointer<1, std::sin>() },
		{ "cos", function_pointer<1, std::cos>() },
		{ "tan", function_pointer<1, std::tan>() },
		{ "arccos", function_pointer<1, std::acos>() },
		{ "arcsin", function_pointer<1, std::asin>() },
		{ "arctan", function_pointer<1, std::atan>() },

		{ "hypot", {{2, 3}, hypot} },
#ifdef CALC_TESTS_ENABLED
		{ "parabola", {{1, 1}, parabola} },
#endif

		{ "sh", function_pointer<1, std::sinh>() },
		{ "ch", function_pointer<1, std::cosh>() },
		{ "tanh", function_pointer<1, std::tanh>() },
		{ "asinh", function_pointer<1, std::asinh>() },
		{ "acosh", function_pointer<1, std::acosh>() },
		{ "atanh", function_pointer<1, std::atanh>() },

		{ "ln", function_pointer<1, std::log>() },
		{ "log", function_pointer<1, std::log>() },
		{ "log1p", function_pointer<1, std::log1p>() },

		{ "lg", function_pointer<1, std::log10>() },
		{ "log10", function_pointer<1, std::log10>() },

		{ "lb", function_pointer<1, std::log2>() },
		{ "log2", function_pointer<1, std::log2>() },

		{ "sqrt", function_pointer<1, std::sqrt>() },
		{ "cbrt", function_pointer<1, std::cbrt>() },
		{ "pow", function_pointer<2, pow>() },
		{ "exp", function_pointer<1, std::exp>() },
		{ "expm1", function_pointer<1, std::expm1>() },
#ifdef CALC_TESTS_ENABLED
		{ "exp2", function_pointer<1, std::exp2>() },
#endif	

#ifdef CALC_TESTS_ENABLED
		{ "fma", function_pointer<3, std::fma>() },
#endif
#ifdef CALC_TESTS_ENABLED
		{ "atan2",function_pointer<2, std::atan2>() },
#endif
		{ "rad", function_pointer<1, rad>() },
		{ "deg", function_pointer<1, deg>() },

		{ "min", {{1, std::numeric_limits<ParamCount>::max()}, min} },
		{ "max", {{1, std::numeric_limits<ParamCount>::max()}, max} },

		{ "abs", function_pointer<1, std::abs>() },

		{ "erf", function_pointer<1, std::erf>() },
		{ "erfc", function_pointer<1, std::erfc>() },

		{ "tgamma", function_pointer<1, std::tgamma>() },
		{ "lgamma", function_pointer<1, std::lgamma>() },

		{ "trunc", function_pointer<1, std::trunc>() },
		{ "round", function_pointer<1, std::round>() },
		{ "ceil",function_pointer<1, std::ceil>() },
		{ "floor",function_pointer<1, std::floor>() },

		{ "mod", function_pointer<2, std::fmod>() },

		//---------------------------------------------------------------------------
		// TODO https://en.cppreference.com/w/cpp/numeric/special_math
		{ "beta", function_pointer<2, std::beta>() },

		{ "assoc_legendre", {{3, 3}, assoc_legendre} },
		{ "assoc_laguerre", {{3, 3}, assoc_laguerre} },

		{ "legendre", {{2, 2}, legendre} },
		{ "laguerre", {{2, 2}, laguerre} },

		{ "comp_ellint_1", function_pointer<1, std::comp_ellint_1>() },
		{ "comp_ellint_2", function_pointer<1, std::comp_ellint_2>() },
		{ "comp_ellint_3", function_pointer<2, std::comp_ellint_3>() },

		{ "cyl_bessel_i", function_pointer<2, std::cyl_bessel_i>() },
		{ "cyl_bessel_j", function_pointer<2, std::cyl_bessel_j>() },
		{ "cyl_bessel_k", function_pointer<2, std::cyl_bessel_k>() },
		{ "cyl_neumann", function_pointer<2, std::cyl_neumann>() },

		{ "sph_bessel", {{2, 2}, sph_bessel}},
		{ "sph_legendre", {{3, 3}, sph_legendre}},
		{ "sph_neumann", {{2, 2}, sph_neumann}},

		{ "ellint_1", function_pointer<2, std::ellint_1>() },
		{ "ellint_2", function_pointer<2, std::ellint_2>() },
		{ "ellint_3", function_pointer<3, std::ellint_3>() },

		{ "expint", function_pointer<1, std::expint>() },

		{ "hermite", {{2, 2}, hermite} },

		{ "riemann_zeta", function_pointer<1, std::riemann_zeta>() },

		//---------------------------------------------------------------------------
#ifdef CALC_TESTS_ENABLED
		{ "distance", {{2, std::numeric_limits<ParamCount>::max()}, distance} },
#endif


		//---------------------------------------------------------------------------
	};

	[[nodiscard]] const map& get() noexcept
	{
		return ids;
	}
}
