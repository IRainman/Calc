/*
 * Copyright 2023-present Elle Solomina, a.rainman on gmail point com
 */

#include "pch.hpp"
#include "identifiers.hpp"
#include "codata2022.hpp"

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

	[[nodiscard]] static constexpr Value min(std::span<Value> params) noexcept
	{
		return std::ranges::min(params);
	}

	[[nodiscard]] static constexpr Value max(std::span<Value> params) noexcept
	{
		return std::ranges::max(params);
	}

	[[nodiscard]] /*constexpr*/ bool compare(const Value a, const Value b) noexcept
	{
		return std::abs(a - b) <= std::numeric_limits<Value>::epsilon() * std::max(std::abs(a), std::abs(b));
	}

	[[nodiscard]] constexpr Value degrees_to_radians(const Value x) noexcept
	{
		return x * std::numbers::pi_v<Value> / 180.0;
	}

	[[nodiscard]] constexpr Value radians_to_degrees(const Value x) noexcept
	{
		return x * 180.0 / std::numbers::pi_v<Value>;
	}

	[[nodiscard]] constexpr Value grad_to_radians(const Value x) noexcept
	{
		return x * std::numbers::pi_v<Value> / 200.0;
	}

	[[nodiscard]] constexpr Value radians_to_grad(const Value x) noexcept
	{
		return x * 200.0 / std::numbers::pi_v<Value>;
	}

	[[nodiscard]] constexpr Value turn_to_radians(const Value x) noexcept
	{
		return x * 2.0 * std::numbers::pi_v<Value>;
	}

	[[nodiscard]] constexpr Value radians_to_turn(const Value x) noexcept
	{
		return x / (2.0 * std::numbers::pi_v<Value>);
	}

	[[nodiscard]] static /*constexpr*/ Value OR(const Value n, const Value m) noexcept
	{
		return static_cast<Value>(static_cast<std::uint64_t>(std::lrint(n)) | static_cast<std::uint64_t>(std::lrint(m)));
	}

	[[nodiscard]] static /*constexpr*/ Value XOR(const Value n, const Value m) noexcept
	{
		return static_cast<Value>(static_cast<std::uint64_t>(std::lrint(n)) ^ static_cast<std::uint64_t>(std::lrint(m)));
	}

	[[nodiscard]] static /*constexpr*/ Value AND(const Value n, const Value m) noexcept
	{
		return static_cast<Value>(static_cast<std::uint64_t>(std::lrint(n)) & static_cast<std::uint64_t>(std::lrint(m)));
	}

	[[nodiscard]] static /*constexpr*/ Value NOT(const Value n) noexcept
	{
		return static_cast<Value>(~(static_cast<std::uint64_t>(std::lrint(n))));
	}

	[[nodiscard]] static /*constexpr*/ Value SHL(const Value n) noexcept
	{
		return static_cast<Value>(static_cast<std::uint64_t>(std::lrint(n)) << 1);
	}

	[[nodiscard]] static /*constexpr*/ Value SHR(const Value n) noexcept
	{
		return static_cast<Value>(static_cast<std::uint64_t>(std::lrint(n)) >> 1);
	}

	[[nodiscard]] static /*constexpr*/ Value SAR(const Value n) noexcept
	{
		return static_cast<Value>(static_cast<std::int64_t>(std::lrint(n)) >> 1);
	}

	[[nodiscard]] static /*constexpr*/ Value factorial(const Value n) noexcept
	{
		long num = std::lrint(n);
		Value result = 1.0;
		if (0.0 <= num && num == n) //-V550
		{
			while (num > 0.0) {
				if (!std::isfinite(result))
				{
					break;
				}
				result = result * num;
				num = num - 1;
			}
			return result;
		}
		return std::numeric_limits<Value>::quiet_NaN();
	}

	[[nodiscard]] static /*constexpr*/ Value permutation(const Value n, const Value r) noexcept
	{
		long num = std::lrint(n);
		long den = std::lrint(r);
		Value result = 1.0;
		if (0.0 <= num && 0.0 <= den && den <= num && num == n && den == r) //-V550
		{
			while (den > 0.0)
			{
				if (!std::isfinite(result))
				{
					break;
				}
				result = result * num;
				--num;
				--den;
			}
			return result;
		}
		return std::numeric_limits<Value>::quiet_NaN();
	}

	[[nodiscard]] static /*constexpr*/ Value combination(const Value n, const Value r) noexcept
	{
		long num = std::lrint(n);
		long den = std::lrint(r);
		Value res1 = 1.0;
		Value res2 = 1.0;
		if (0 <= num && 0 <= den && den <= num && num == n && den == r) //-V550
		{
			while (den > 0)
			{
				if (!std::isfinite(res1))
				{
					break;
				}
				res1 = res1 * num;
				res2 = res2 * den;
				--num;
				--den;
			}
			return res1 / res2;
		}
		return std::numeric_limits<Value>::quiet_NaN();
	}
#if 0
	[[nodiscard]] static constexpr Value accumulate(std::span<Value> params) noexcept
	{
		return std::accumulate(params.begin(), params.end(), 0.0);
	}

	[[nodiscard]] static constexpr Value reduce(std::span<Value> params) noexcept
	{
		return std::reduce(params.begin(), params.end());
	}
#endif
	[[nodiscard]] static constexpr Value hypot(std::span<Value> params) noexcept
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

	[[nodiscard]] static constexpr Value parabola(std::span<Value> params) noexcept
	{
		return params[0] * params[0] + 1.0;
	}

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

	using namespace codata2022;

	static const map ids =
	{
		//---------------------------------------------------------------------------
		{ "inf", constant<std::numeric_limits<Value>::infinity()>() }, // added for optimization, reduce code size and speedup float parsing.
		//---------------------------------------------------------------------------
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
		//---------------------------------------------------------------------------
		// http://physics.nist.gov/constants
		// CODATA2022 constants

		{ "electron_mass", constant<electron_mass>() },
		{ "proton_mass", constant<proton_mass>() },
		{ "neutron_mass", constant<neutron_mass>() },
		{ "deuteron_mass", constant<deuteron_mass>() },
		{ "triton_mass", constant<triton_mass>() },
		{ "alpha_particle_mass", constant<alpha_particle_mass>() },

		{ "electron_mass_energy_J", constant<electron_mass_energy_J>() },
		{ "proton_mass_energy_J", constant<proton_mass_energy_J>() },
		{ "neutron_mass_energy_J", constant<neutron_mass_energy_J>() },
		{ "deuteron_mass_energy_J", constant<deuteron_mass_energy_J>() },
		{ "triton_mass_energy_J", constant<triton_mass_energy_J>() },
		{ "alpha_particle_mass_energy_J", constant<alpha_particle_mass_energy_J>() },

		{ "electron_mass_energy_MeV", constant<electron_mass_energy_MeV>() },
		{ "proton_mass_energy_MeV", constant<proton_mass_energy_MeV>() },
		{ "neutron_mass_energy_MeV", constant<neutron_mass_energy_MeV>() },
		{ "deuteron_mass_energy_MeV", constant<deuteron_mass_energy_MeV>() },
		{ "triton_mass_energy_MeV", constant<triton_mass_energy_MeV>() },
		{ "alpha_particle_mass_energy_MeV", constant<alpha_particle_mass_energy_MeV>() },

		{ "proton_electron_mass_ratio", constant<proton_electron_mass_ratio>() },
		{ "neutron_proton_mass_ratio", constant<neutron_proton_mass_ratio>() },
		{ "deuteron_electron_mass_ratio", constant<deuteron_electron_mass_ratio>() },
		{ "alpha_electron_mass_ratio", constant<alpha_electron_mass_ratio>() },

		{ "classical_electron_radius", constant<classical_electron_radius>() },
		{ "electron_compton_wavelength", constant<electron_compton_wavelength>() },
		{ "proton_compton_wavelength", constant<proton_compton_wavelength>() },
		{ "neutron_compton_wavelength", constant<neutron_compton_wavelength>() },
		{ "reduced_compton_wavelength", constant<reduced_compton_wavelength>() },

		{ "bohr_magneton", constant<bohr_magneton>() },
		{ "nuclear_magneton", constant<nuclear_magneton>() },
		{ "electron_magnetic_moment", constant<electron_magnetic_moment>() },
		{ "proton_magnetic_moment", constant<proton_magnetic_moment>() },
		{ "neutron_magnetic_moment", constant<neutron_magnetic_moment>() },

		{ "electron_gyro_ratio", constant<electron_gyro_ratio>() },
		{ "proton_gyro_ratio", constant<proton_gyro_ratio>() },
		{ "neutron_gyro_ratio", constant<neutron_gyro_ratio>() },

		{ "electron_g_factor", constant<electron_g_factor>() },
		{ "proton_g_factor", constant<proton_g_factor>() },
		{ "neutron_g_factor", constant<neutron_g_factor>() },

		{ "proton_to_bohr_moment_ratio", constant<proton_to_bohr_moment_ratio>() },
		{ "neutron_to_bohr_moment_ratio", constant<neutron_to_bohr_moment_ratio>() },

		{ "neutron_proton_mass_difference", constant<neutron_proton_mass_difference>() },
		{ "neutron_proton_mass_diff_energy_J", constant<neutron_proton_mass_diff_energy_J>() },
		{ "neutron_proton_mass_diff_energy_MeV", constant<neutron_proton_mass_diff_energy_MeV>() },

		{ "atomic_mass_unit", constant<atomic_mass_unit>() },
		{ "bohr_radius", constant<bohr_radius>() },
		{ "hartree_energy_J", constant<hartree_energy_J>() },
		{ "hartree_energy_eV", constant<hartree_energy_eV>() },

		{ "atomic_unit_length", constant<atomic_unit_length>() },
		{ "atomic_unit_mass", constant<atomic_unit_mass>() },
		{ "atomic_unit_time", constant<atomic_unit_time>() },
		{ "atomic_unit_velocity", constant<atomic_unit_velocity>() },

		{ "atomic_unit_energy_J", constant<atomic_unit_energy_J>() },
		{ "atomic_unit_action_Js", constant<atomic_unit_action_Js>() },
		{ "atomic_unit_momentum", constant<atomic_unit_momentum>() },

		{ "atomic_unit_charge", constant<atomic_unit_charge>() },
		{ "atomic_unit_dipole_moment", constant<atomic_unit_dipole_moment>() },
		{ "atomic_unit_electric_field", constant<atomic_unit_electric_field>() },
		{ "atomic_unit_potential", constant<atomic_unit_potential>() },
		{ "atomic_unit_force", constant<atomic_unit_force>() },
		{ "atomic_unit_polarizability", constant<atomic_unit_polarizability>() },
		{ "atomic_unit_magnetic_dipole", constant<atomic_unit_magnetic_dipole>() },
		{ "atomic_unit_flux_density", constant<atomic_unit_flux_density>() },
		{ "atomic_unit_current", constant<atomic_unit_current>() },

		{ "deuteron_g_factor", constant<deuteron_g_factor>() },
		{ "deuteron_magnetic_moment", constant<deuteron_magnetic_moment>() },
		{ "deuteron_compton_wavelength", constant<deuteron_compton_wavelength>() },

		{ "triton_g_factor", constant<triton_g_factor>() },
		{ "triton_magnetic_moment", constant<triton_magnetic_moment>() },
		{ "triton_compton_wavelength", constant<triton_compton_wavelength>() },

		{ "helion_mass", constant<helion_mass>() },
		{ "helion_mass_energy_J", constant<helion_mass_energy_J>() },
		{ "helion_mass_energy_MeV", constant<helion_mass_energy_MeV>() },
		{ "helion_g_factor", constant<helion_g_factor>() },
		{ "helion_magnetic_moment", constant<helion_magnetic_moment>() },
		{ "helion_compton_wavelength", constant<helion_compton_wavelength>() },

		{ "alpha_mass", constant<alpha_mass>() },
		{ "alpha_mass_energy_J", constant<alpha_mass_energy_J>() },
		{ "alpha_mass_energy_MeV", constant<alpha_mass_energy_MeV>() },
		{ "alpha_g_factor", constant<alpha_g_factor>() },
		{ "alpha_magnetic_moment", constant<alpha_magnetic_moment>() },
		{ "alpha_compton_wavelength", constant<alpha_compton_wavelength>() },

		{ "m_e", constant<m_e>() },
		{ "a0", constant<a0>() },
		{ "t_a", constant<t_a>() },
		{ "Eh", constant<Eh>() },
		{ "E_h", constant<E_h>() },
		{ "V_h", constant<V_h>() },
		{ "e_atomic", constant<e>() }, // use distinct name to avoid collision with math "e"
		{ "I_h", constant<I_h>() },
		{ "mu_h", constant<mu_h>() },

		{ "l_p", constant<l_p>() },
		{ "m_p", constant<m_p>() },
		{ "t_p", constant<t_p>() },
		{ "q_p", constant<q_p>() },
		{ "T_p", constant<T_p>() },
		{ "E_p", constant<E_p>() },

		{ "au", constant<au>() },
		{ "ly", constant<ly>() },
		{ "pc", constant<pc>() },
		{ "M_sun", constant<M_sun>() },
		{ "M_earth", constant<M_earth>() },
		{ "M_jupiter", constant<M_jupiter>() },
		{ "R_sun", constant<R_sun>() },
		{ "R_earth", constant<R_earth>() },
		{ "G", constant<G>() },
		{ "g0", constant<g0>() },

		{ "c", constant<c>() },
		{ "mu0", constant<mu0>() },
		{ "epsilon0", constant<epsilon0>() },
		{ "h", constant<h>() },
		{ "hbar", constant<hbar>() },
		{ "e_charge", constant<e>() }, // use distinct name to avoid collision with math "e"
		{ "NA", constant<NA>() },
		{ "kB", constant<kB>() },
		{ "F", constant<F>() },
		{ "R", constant<R>() },
		{ "sigma", constant<sigma>() },
		{ "R_inf", constant<R_inf>() },
		{ "alpha", constant<alpha>() },
		{ "G0", constant<G0>() },
		{ "RK", constant<RK>() },
		{ "KJ", constant<KJ>() },
		{ "phi0", constant<phi0>() },
		{ "atm", constant<atm>() },
		{ "u", constant<u>() },
		{ "lambda_e", constant<lambda_e>() },
		{ "lambda_bar_e", constant<lambda_bar_e>() },
		{ "me", constant<me>() },
		{ "me_u", constant<me_u>() },
		{ "me_MeV", constant<me_MeV>() },
		{ "mp", constant<mp>() },
		{ "mp_u", constant<mp_u>() },
		{ "mp_MeV", constant<mp_MeV>() },
		{ "mn", constant<mn>() },
		{ "mn_u", constant<mn_u>() },
		{ "mn_MeV", constant<mn_MeV>() },
		{ "md", constant<md>() },
		{ "md_u", constant<md_u>() },
		{ "md_MeV", constant<md_MeV>() },
		{ "malpha", constant<malpha>() },
		{ "malpha_u", constant<malpha_u>() },
		{ "malpha_MeV", constant<malpha_MeV>() },
		{ "mh", constant<mh>() },
		{ "mh_u", constant<mh_u>() },
		{ "mh_MeV", constant<mh_MeV>() },
		{ "mu", constant<mu>() },
		{ "me_mp", constant<me_mp>() },
		{ "me_mm", constant<me_mm>() },
		{ "mn_mp", constant<mn_mp>() },
		{ "md_mp", constant<md_mp>() },
		{ "muB", constant<muB>() },
		{ "muN", constant<muN>() },
		{ "mue", constant<mue>() },
		{ "mup", constant<mup>() },
		{ "mun", constant<mun>() },
		{ "mud", constant<mud>() },
		{ "mualpha", constant<mualpha>() },
		{ "re", constant<re>() },
		{ "sigmae", constant<sigmae>() },

		// TODO additional constants
		{ "Z0", constant<376.730313461>() }, // characteristic impedance of vacuum (Ohm)

		//---------------------------------------------------------------------------
		// https://en.cppreference.com/w/cpp/numeric/math
		{ "sin", function_pointer<1, std::sin>() },
		{ "cos", function_pointer<1, std::cos>() },
		{ "tan", function_pointer<1, std::tan>() },
		{ "arccos", function_pointer<1, std::acos>() },
		{ "arcsin", function_pointer<1, std::asin>() },
		{ "arctan", function_pointer<1, std::atan>() },

		{ "factorial", function_pointer <1, factorial>() },
#if 0 //def CALC_TESTS_ENABLED
		{ "gcd", function_pointer<2, std::gcd>() },
		{ "lcm", function_pointer<2, std::lcm>() },
#endif

		{ "permutation", function_pointer <2, permutation>() },
		{ "P",            function_pointer <2, permutation>() },
		{ "combination", function_pointer <2, combination>() },
		{ "C",            function_pointer <2, combination>() },
#if 0
		{ "accumulate", {{2, std::numeric_limits<ParamCount>::max()}, accumulate} },
		{ "reduce", {{2, std::numeric_limits<ParamCount>::max()}, reduce} },
#endif
		{ "atan2",function_pointer<2, std::atan2>() },
		{ "hypot", {{2, 3}, hypot} },
		{ "parabola", {{1, 1}, parabola} },
		{ "distance", {{2, std::numeric_limits<ParamCount>::max()}, distance} },

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
		{ "degrees_to_radians", function_pointer<1, degrees_to_radians>() },
		{ "rad",                function_pointer<1, degrees_to_radians>() },

		{ "radians_to_degrees", function_pointer<1, radians_to_degrees>() },
		{ "deg",                function_pointer<1, radians_to_degrees>() },

		{ "grad_to_radians", function_pointer<1, grad_to_radians>() },
		{ "radians_to_grad", function_pointer<1, radians_to_grad>() },

		{ "turn_to_radians", function_pointer<1, turn_to_radians>() },
		{ "radians_to_turn", function_pointer<1, radians_to_turn>() },

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

#if 0 //def CALC_TESTS_ENABLED
		{ "clamp", function_pointer<3, std::clamp>() },
		{ "midpoint", function_pointer <2, std::midpoint>() },
		{ "lerp", function_pointer<3, std::std::lerp>() },
#endif
		{ "not", function_pointer<1, NOT>() },
		{ "and", function_pointer<2, AND>() },
		{ "or",  function_pointer<2, OR>() },
		{ "xor", function_pointer<2, XOR>() },
		{ "shl", function_pointer<1, SHL>() },
		{ "shr", function_pointer<1, SHR>() },
		{ "sar", function_pointer<1, SAR>() },
#if 0 //def CALC_TESTS_ENABLED
		{ "rotl", function_pointer<2, std::rotl>() },
		{ "rotr", function_pointer<2, std::rotr>() },
		{ "countl_zero", function_pointer<1, std::countl_zero>() },
		{ "countl_one", function_pointer<1, std::countl_one>() },
		{ "countr_zero", function_pointer<1, std::countr_zero>() },
		{ "countr_one", function_pointer<1, std::countr_one>() },
		{ "popcount", function_pointer<1, std::popcount>() },
#endif
		//---------------------------------------------------------------------------
		// https://en.cppreference.com/w/cpp/numeric/special_math
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
	};

	[[nodiscard]] const map& get() noexcept
	{
		return ids;
	}
}
