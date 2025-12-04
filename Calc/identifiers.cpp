// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

/*
 * Copyright 2023-2025 Solomina Elle, a.rainman on gmail point com
 */

#include "pch.h"
#include "identifiers.h"
#include "codata2022.hpp"

namespace Identifiers
{
	using namespace codata2022;

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
	[[nodiscard]] constexpr auto call_fn(Fn fn, std::span<const Value> params, std::index_sequence<Is...>) noexcept
	{
		return fn(params[Is]...);
	}

	template<const ParamCount N, WrappedFn<N> wrappedFn>
	[[nodiscard]] constexpr auto function_pointer_impl(std::span<Value> params) noexcept
	{
		return call_fn(wrappedFn, params, std::make_index_sequence<N>());
	}

	template<const ParamCount N, WrappedFn<N> wrappedFn>
	[[nodiscard]] consteval Fn function_pointer() noexcept
	{
		return { { N, N }, function_pointer_impl<N, wrappedFn> };
	}

	template <const Value value>
	[[nodiscard]] constexpr auto constant_impl(std::span<Value>) noexcept
	{
		return value;
	}

	template <const Value value>
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
		{ "inf", constant<std::numeric_limits<Value>::infinity()>() }, // added for optimization, reduce code size and speedup float parsing.
		//---------------------------------------------------------------------------
		// http://physics.nist.gov/constants
		// CODATA2022 constants (single, canonical set)
		{ "c", constant<physical::c>() },
		{ "mu0", constant<physical::mu0>() },
		{ "epsilon0", constant<physical::epsilon0>() },
		{ "h", constant<physical::h>() },
		{ "hbar", constant<physical::hbar>() },
		{ "e_charge", constant<physical::e>() }, // use distinct name to avoid collision with math "e"
		{ "NA", constant<physical::NA>() },
		{ "kB", constant<physical::kB>() },
		{ "F", constant<physical::F>() },
		{ "R", constant<physical::R>() },
		{ "sigma", constant<physical::sigma>() },
		{ "R_inf", constant<physical::R_inf>() },
		{ "alpha", constant<physical::alpha>() },
		{ "G0", constant<physical::G0>() },
		{ "RK", constant<physical::RK>() },
		{ "KJ", constant<physical::KJ>() },
		{ "phi0", constant<physical::phi0>() },
		{ "atm", constant<physical::atm>() },
		{ "eV", constant<physical::eV>() },
		{ "u", constant<physical::u>() },
		{ "a0", constant<physical::a0>() },
		{ "Eh", constant<physical::Eh>() },
		{ "muB", constant<physical::muB>() },
		{ "lambda_e", constant<physical::lambda_e>() },
		{ "lambda_bar_e", constant<physical::lambda_bar_e>() },
		{ "R", constant<physical::R>() },
		{ "F", constant<physical::F>() },
		{ "KJ", constant<physical::KJ>() },
		{ "RK", constant<physical::RK>() },
		{ "G0", constant<physical::G0>() },
		// Physical (mass/charge/atomic units - the second 'physical' namespace in codata file)
		{ "me", constant<physical::me>() },
		{ "me_u", constant<physical::me_u>() },
		{ "me_MeV", constant<physical::me_MeV>() },
		{ "mp", constant<physical::mp>() },
		{ "mp_u", constant<physical::mp_u>() },
		{ "mp_MeV", constant<physical::mp_MeV>() },
		{ "mn", constant<physical::mn>() },
		{ "mn_u", constant<physical::mn_u>() },
		{ "mn_MeV", constant<physical::mn_MeV>() },
		{ "md", constant<physical::md>() },
		{ "md_u", constant<physical::md_u>() },
		{ "md_MeV", constant<physical::md_MeV>() },
		{ "malpha", constant<physical::malpha>() },
		{ "malpha_u", constant<physical::malpha_u>() },
		{ "malpha_MeV", constant<physical::malpha_MeV>() },
		{ "mh", constant<physical::mh>() },
		{ "mh_u", constant<physical::mh_u>() },
		{ "mh_MeV", constant<physical::mh_MeV>() },
		{ "mu", constant<physical::mu>() },
		{ "me_mp", constant<physical::me_mp>() },
		{ "me_mm", constant<physical::me_mm>() },
		{ "mn_mp", constant<physical::mn_mp>() },
		{ "md_mp", constant<physical::md_mp>() },
		{ "muB", constant<physical::muB>() },
		{ "muN", constant<physical::muN>() },
		{ "mue", constant<physical::mue>() },
		{ "mup", constant<physical::mup>() },
		{ "mun", constant<physical::mun>() },
		{ "mud", constant<physical::mud>() },
		{ "mualpha", constant<physical::mualpha>() },
		{ "lambdaC", constant<physical::lambdaC>() },
		{ "lambdaC_e", constant<physical::lambdaC_e>() },
		{ "re", constant<physical::re>() },
		{ "sigmae", constant<physical::sigmae>() },
#ifdef CALC_TESTS_ENABLED
		// Particle constants
		{ "electron_mass", constant<particle::electron_mass>() },
		{ "proton_mass", constant<particle::proton_mass>() },
		{ "neutron_mass", constant<particle::neutron_mass>() },
		{ "deuteron_mass", constant<particle::deuteron_mass>() },
		{ "triton_mass", constant<particle::triton_mass>() },
		{ "alpha_particle_mass", constant<particle::alpha_particle_mass>() },

		{ "electron_mass_energy_J", constant<particle::electron_mass_energy_J>() },
		{ "proton_mass_energy_J", constant<particle::proton_mass_energy_J>() },
		{ "neutron_mass_energy_J", constant<particle::neutron_mass_energy_J>() },
		{ "deuteron_mass_energy_J", constant<particle::deuteron_mass_energy_J>() },
		{ "triton_mass_energy_J", constant<particle::triton_mass_energy_J>() },
		{ "alpha_particle_mass_energy_J", constant<particle::alpha_particle_mass_energy_J>() },

		{ "electron_mass_energy_MeV", constant<particle::electron_mass_energy_MeV>() },
		{ "proton_mass_energy_MeV", constant<particle::proton_mass_energy_MeV>() },
		{ "neutron_mass_energy_MeV", constant<particle::neutron_mass_energy_MeV>() },
		{ "deuteron_mass_energy_MeV", constant<particle::deuteron_mass_energy_MeV>() },
		{ "triton_mass_energy_MeV", constant<particle::triton_mass_energy_MeV>() },
		{ "alpha_particle_mass_energy_MeV", constant<particle::alpha_particle_mass_energy_MeV>() },

		{ "proton_electron_mass_ratio", constant<particle::proton_electron_mass_ratio>() },
		{ "neutron_proton_mass_ratio", constant<particle::neutron_proton_mass_ratio>() },
		{ "deuteron_electron_mass_ratio", constant<particle::deuteron_electron_mass_ratio>() },
		{ "alpha_electron_mass_ratio", constant<particle::alpha_electron_mass_ratio>() },

		{ "classical_electron_radius", constant<particle::classical_electron_radius>() },
		{ "electron_compton_wavelength", constant<particle::electron_compton_wavelength>() },
		{ "proton_compton_wavelength", constant<particle::proton_compton_wavelength>() },
		{ "neutron_compton_wavelength", constant<particle::neutron_compton_wavelength>() },
		{ "reduced_compton_wavelength", constant<particle::reduced_compton_wavelength>() },

		{ "bohr_magneton", constant<particle::bohr_magneton>() },
		{ "nuclear_magneton", constant<particle::nuclear_magneton>() },
		{ "electron_magnetic_moment", constant<particle::electron_magnetic_moment>() },
		{ "proton_magnetic_moment", constant<particle::proton_magnetic_moment>() },
		{ "neutron_magnetic_moment", constant<particle::neutron_magnetic_moment>() },

		{ "electron_gyro_ratio", constant<particle::electron_gyro_ratio>() },
		{ "proton_gyro_ratio", constant<particle::proton_gyro_ratio>() },
		{ "neutron_gyro_ratio", constant<particle::neutron_gyro_ratio>() },

		{ "electron_g_factor", constant<particle::electron_g_factor>() },
		{ "proton_g_factor", constant<particle::proton_g_factor>() },
		{ "neutron_g_factor", constant<particle::neutron_g_factor>() },

		{ "proton_to_bohr_moment_ratio", constant<particle::proton_to_bohr_moment_ratio>() },
		{ "neutron_to_bohr_moment_ratio", constant<particle::neutron_to_bohr_moment_ratio>() },

		{ "neutron_proton_mass_difference", constant<particle::neutron_proton_mass_difference>() },
		{ "neutron_proton_mass_diff_energy_J", constant<particle::neutron_proton_mass_diff_energy_J>() },
		{ "neutron_proton_mass_diff_energy_MeV", constant<particle::neutron_proton_mass_diff_energy_MeV>() },

		// Atomic / atomic-unit constants (first atomic namespace)
		{ "atomic_mass_unit", constant<atomic::atomic_mass_unit>() },
		{ "atomic_mass_unit_u", constant<atomic::atomic_mass_unit_u>() },
		{ "bohr_radius", constant<atomic::bohr_radius>() },
		{ "hartree_energy_J", constant<atomic::hartree_energy_J>() },
		{ "hartree_energy_eV", constant<atomic::hartree_energy_eV>() },

		{ "atomic_unit_length", constant<atomic::atomic_unit_length>() },
		{ "atomic_unit_mass", constant<atomic::atomic_unit_mass>() },
		{ "atomic_unit_time", constant<atomic::atomic_unit_time>() },
		{ "atomic_unit_velocity", constant<atomic::atomic_unit_velocity>() },

		{ "atomic_unit_energy_J", constant<atomic::atomic_unit_energy_J>() },
		{ "atomic_unit_action_Js", constant<atomic::atomic_unit_action_Js>() },
		{ "atomic_unit_momentum", constant<atomic::atomic_unit_momentum>() },

		{ "atomic_unit_charge", constant<atomic::atomic_unit_charge>() },
		{ "atomic_unit_dipole_moment", constant<atomic::atomic_unit_dipole_moment>() },
		{ "atomic_unit_electric_field", constant<atomic::atomic_unit_electric_field>() },
		{ "atomic_unit_potential", constant<atomic::atomic_unit_potential>() },
		{ "atomic_unit_force", constant<atomic::atomic_unit_force>() },
		{ "atomic_unit_polarizability", constant<atomic::atomic_unit_polarizability>() },
		{ "atomic_unit_magnetic_dipole", constant<atomic::atomic_unit_magnetic_dipole>() },
		{ "atomic_unit_flux_density", constant<atomic::atomic_unit_flux_density>() },
		{ "atomic_unit_current", constant<atomic::atomic_unit_current>() },

		// Nuclear / isotope constants
		{ "deuteron_mass", constant<nuclear::deuteron_mass>() },
		{ "deuteron_mass_energy_J", constant<nuclear::deuteron_mass_energy_J>() },
		{ "deuteron_mass_energy_MeV", constant<nuclear::deuteron_mass_energy_MeV>() },
		{ "deuteron_g_factor", constant<nuclear::deuteron_g_factor>() },
		{ "deuteron_magnetic_moment", constant<nuclear::deuteron_magnetic_moment>() },
		{ "deuteron_compton_wavelength", constant<nuclear::deuteron_compton_wavelength>() },

		{ "triton_mass", constant<nuclear::triton_mass>() },
		{ "triton_mass_energy_J", constant<nuclear::triton_mass_energy_J>() },
		{ "triton_mass_energy_MeV", constant<nuclear::triton_mass_energy_MeV>() },
		{ "triton_g_factor", constant<nuclear::triton_g_factor>() },
		{ "triton_magnetic_moment", constant<nuclear::triton_magnetic_moment>() },
		{ "triton_compton_wavelength", constant<nuclear::triton_compton_wavelength>() },

		{ "helion_mass", constant<nuclear::helion_mass>() },
		{ "helion_mass_energy_J", constant<nuclear::helion_mass_energy_J>() },
		{ "helion_mass_energy_MeV", constant<nuclear::helion_mass_energy_MeV>() },
		{ "helion_g_factor", constant<nuclear::helion_g_factor>() },
		{ "helion_magnetic_moment", constant<nuclear::helion_magnetic_moment>() },
		{ "helion_compton_wavelength", constant<nuclear::helion_compton_wavelength>() },

		{ "alpha_mass", constant<nuclear::alpha_mass>() },
		{ "alpha_mass_energy_J", constant<nuclear::alpha_mass_energy_J>() },
		{ "alpha_mass_energy_MeV", constant<nuclear::alpha_mass_energy_MeV>() },
		{ "alpha_g_factor", constant<nuclear::alpha_g_factor>() },
		{ "alpha_magnetic_moment", constant<nuclear::alpha_magnetic_moment>() },
		{ "alpha_compton_wavelength", constant<nuclear::alpha_compton_wavelength>() },

		{ "proton_mass_energy_MeV", constant<nuclear::proton_mass_energy_MeV>() },
		{ "neutron_mass_energy_MeV", constant<nuclear::neutron_mass_energy_MeV>() },
		{ "nuclear_magneton", constant<nuclear::nuclear_magneton>() },
#endif
		// Atomic (second atomic namespace - atomic units)
		{ "m_e", constant<atomic::m_e>() },
		{ "a0", constant<atomic::a0>() },
		{ "t_a", constant<atomic::t_a>() },
		{ "Eh", constant<atomic::Eh>() },
		{ "E_h", constant<atomic::E_h>() },
		{ "V_h", constant<atomic::V_h>() },
		{ "e_atomic", constant<atomic::e>() }, // use distinct name to avoid collision with math "e"
		{ "I_h", constant<atomic::I_h>() },
		{ "mu_h", constant<atomic::mu_h>() },

		// Planck units
		{ "l_p", constant<planck::l_p>() },
		{ "m_p", constant<planck::m_p>() },
		{ "t_p", constant<planck::t_p>() },
		{ "q_p", constant<planck::q_p>() },
		{ "T_p", constant<planck::T_p>() },
		{ "E_p", constant<planck::E_p>() },

		// Astronomy
		{ "au", constant<astronomy::au>() },
		{ "ly", constant<astronomy::ly>() },
		{ "pc", constant<astronomy::pc>() },
		{ "M_sun", constant<astronomy::M_sun>() },
		{ "M_earth", constant<astronomy::M_earth>() },
		{ "M_jupiter", constant<astronomy::M_jupiter>() },
		{ "R_sun", constant<astronomy::R_sun>() },
		{ "R_earth", constant<astronomy::R_earth>() },
		{ "G", constant<astronomy::G>() },
		{ "g0", constant<astronomy::g0>() },

		// TODO additional constant
		{ "J", constant<3.058198247456354132564564787888767>() }, // Constants of Gauss field
		{ "Z_0", constant<376.7303134617706554681984004203193082686>() }, // characteristic impedance of vacuum (Ohm)

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
