#ifndef CODATA2022_HPP
#define CODATA2022_HPP

/*
 * Copyright 2025 Solomina Elle, a.rainman on gmail point com
 */

// CODATA 2022 – Fundamental Physical Constants --- Complete Listing
// From:  http://physics.nist.gov/constants (May 2024)

// -------------------------------------------------------------
// PHYSICAL CONSTANTS (SI)
// -------------------------------------------------------------
namespace codata2022 {
    // -------------------------------------------------------------
    // NUCLEAR / ISOTOPE CONSTANTS
    // -------------------------------------------------------------

    // — Deuteron
    static constexpr Value deuteron_mass = 3.3435837768e-27;   // kg
    static constexpr Value deuteron_mass_energy_J = 3.00506323491e-10;  // J
    static constexpr Value deuteron_mass_energy_MeV = 1875.61294500;      // MeV
    static constexpr Value deuteron_g_factor = 0.8574382338;       // dimensionless
    static constexpr Value deuteron_magnetic_moment = 4.33073548e-27;     // J/T
    static constexpr Value deuteron_compton_wavelength = 1.32140985360e-15;  // m

    // — Triton
    static constexpr Value triton_mass = 5.0073567512e-27;
    static constexpr Value triton_mass_energy_J = 4.5003878119e-10;
    static constexpr Value triton_mass_energy_MeV = 2808.92113668;
    static constexpr Value triton_g_factor = 5.957924895;
    static constexpr Value triton_magnetic_moment = 2.97896244e-26;
    static constexpr Value triton_compton_wavelength = 1.32140985360e-15;  // m

    // — Helion (3He nucleus)
    static constexpr Value helion_mass = 5.0064127862e-27;
    static constexpr Value helion_mass_energy_J = 4.499e-10;
    static constexpr Value helion_mass_energy_MeV = 2808.391586;
    static constexpr Value helion_g_factor = -4.255250613;
    static constexpr Value helion_magnetic_moment = -2.1276253e-26;
    static constexpr Value helion_compton_wavelength = 1.321409e-15;

    // — Alpha particle (4He nucleus)
    static constexpr Value alpha_mass = 6.6446573450e-27;
    static constexpr Value alpha_mass_energy_J = 5.9719201997e-10;
    static constexpr Value alpha_mass_energy_MeV = 3727.3794118;
    static constexpr Value alpha_g_factor = 0.0;
    static constexpr Value alpha_magnetic_moment = 0.0; // spin-0, zero magnetic moment
    static constexpr Value alpha_compton_wavelength = 1.32140985360e-15; // approximate

    // — Nuclear magneton
    static constexpr Value nuclear_magneton = 5.0507837393e-27; // J/T

    // -------------------------------------------------------------
    // PARTICLE MASSES, MAGNETIC MOMENTS, COMPTON WAVELENGTHS, ETC.
    // -------------------------------------------------------------

    // — Masses (kg)
    static constexpr Value electron_mass = 9.1093837139e-31;     // electron mass
    static constexpr Value proton_mass = 1.67262192595e-27;    // proton mass
    static constexpr Value neutron_mass = 1.67492750056e-27;    // neutron mass
    //static constexpr Value deuteron_mass = 3.3435837768e-27;     // deuteron mass
    //static constexpr Value triton_mass = 5.0073567512e-27;     // triton mass
    static constexpr Value alpha_particle_mass = alpha_mass;     // alpha-particle mass

    // — Mass energy equivalents (J)
    static constexpr Value electron_mass_energy_J = 8.1871057880e-14;
    static constexpr Value proton_mass_energy_J = 1.50327761802e-10;
    static constexpr Value neutron_mass_energy_J = 1.50534976514e-10;
    //static constexpr Value deuteron_mass_energy_J = 3.00506323491e-10;
    //static constexpr Value triton_mass_energy_J = 4.5003878119e-10;
    static constexpr Value alpha_particle_mass_energy_J = 5.9719201997e-10;

    // — Mass energy equivalents (MeV)
    static constexpr Value electron_mass_energy_MeV = 0.51099895069;
    static constexpr Value proton_mass_energy_MeV = 938.27208943;
    static constexpr Value neutron_mass_energy_MeV = 939.56542194;
    //static constexpr Value deuteron_mass_energy_MeV = 1875.61294500;
    //static constexpr Value triton_mass_energy_MeV = 2808.92113668;
    static constexpr Value alpha_particle_mass_energy_MeV = alpha_mass_energy_MeV;

    // — Mass ratios
    static constexpr Value proton_electron_mass_ratio = 1836.152673426;
    static constexpr Value neutron_proton_mass_ratio = 1.00137841946;
    static constexpr Value deuteron_electron_mass_ratio = 3670.482967655;
    static constexpr Value alpha_electron_mass_ratio = 7294.29954171;

    // — Classical radius & Compton wavelengths
    static constexpr Value classical_electron_radius = 2.8179403205e-15;  // m
    static constexpr Value electron_compton_wavelength = 2.42631023538e-12; // m
    static constexpr Value proton_compton_wavelength = 1.32140985360e-15; // m
    static constexpr Value neutron_compton_wavelength = 1.31959090382e-15; // m
    static constexpr Value reduced_compton_wavelength = 3.8615926744e-13;  // m (reduced ? / (m c))

    // — Magnetic moments / gyromagnetic ratios
    static constexpr Value bohr_magneton = 9.2740100657e-24;   // J T^-1
    //static constexpr Value nuclear_magneton = 5.0507837393e-27;   // J T^-1
    static constexpr Value electron_magnetic_moment = -9.2847646917e-24;  // J T^-1
    static constexpr Value proton_magnetic_moment = 1.41060679545e-26;  // J T^-1
    static constexpr Value neutron_magnetic_moment = -9.6623653e-27;     // J T^-1

    // — Gyromagnetic ratios (s^-1 T^-1)
    static constexpr Value electron_gyro_ratio = 1.76085962784e11;
    static constexpr Value proton_gyro_ratio = 2.6752218708e8;
    static constexpr Value neutron_gyro_ratio = 1.83247174e8;

    // — g-factors (dimensionless)
    static constexpr Value electron_g_factor = -2.00231930436092;
    static constexpr Value proton_g_factor = 5.5856946893;
    static constexpr Value neutron_g_factor = -3.82608552;

    // — Magnetic moment to Bohr / nuclear magneton ratios
    static constexpr Value proton_to_bohr_moment_ratio = 1.52103220230e-3;
    static constexpr Value neutron_to_bohr_moment_ratio = -1.04187565e-3;

    // — Mass differences
    static constexpr Value neutron_proton_mass_difference = 2.30557461e-30;   // kg
    static constexpr Value neutron_proton_mass_diff_energy_J = 2.07214712e-13;   // J
    static constexpr Value neutron_proton_mass_diff_energy_MeV = 1.29333251;       // MeV

    // -------------------------------------------------------------
    // ATOMIC / ATOMIC-UNIT CONSTANTS
    // -------------------------------------------------------------

    // — Atomic mass unit
    static constexpr Value atomic_mass_unit = 1.66053906892e-27;   // kg

    // — Bohr radius
    static constexpr Value bohr_radius = 5.29177210544e-11;   // m

    // — Hartree energy (atomic unit of energy)
    static constexpr Value hartree_energy_J = 4.3597447222060e-18; // J
    static constexpr Value hartree_energy_eV = 27.211386245981;     // eV

    // — Atomic unit of length, mass, time, velocity
    static constexpr Value atomic_unit_length = bohr_radius;   // m (a0)
    static constexpr Value atomic_unit_mass = electron_mass;    // kg (electron mass)
    static constexpr Value atomic_unit_time = 2.4188843265864e-17; // s
    static constexpr Value atomic_unit_velocity = 2.18769126216e6;     // m s^-1

    // — Atomic unit of energy / action / momentum
    static constexpr Value atomic_unit_energy_J = hartree_energy_J; // J
    static constexpr Value atomic_unit_action_Js = 1.0545718176461565e-34; // J s (?)
    static constexpr Value atomic_unit_momentum = 1.99285191545e-24;   // kg m s^-1

    // — Atomic unit of charge (same as elementary charge)
    static constexpr Value atomic_unit_charge = 1.602176634e-19;      // C

    // — Atomic unit of electric dipole moment
    static constexpr Value atomic_unit_dipole_moment = 8.4783536198e-30;    // C m

    // — Atomic unit of electric field
    static constexpr Value atomic_unit_electric_field = 5.14220675112e11;   // V m^-1

    // — Atomic unit of electric potential (Hartree / e)
    static constexpr Value atomic_unit_potential = hartree_energy_eV;       // V

    // — Atomic unit of force
    static constexpr Value atomic_unit_force = 8.2387235038e-8;       // N

    // — Atomic unit of electric polarizability
    static constexpr Value atomic_unit_polarizability = 1.64877727212e-41; // C^2 m^2 J^-1

    // — Atomic unit of magnetic dipole moment
    static constexpr Value atomic_unit_magnetic_dipole = 1.85480201315e-23; // J T^-1

    // — Atomic unit of magnetic flux density
    static constexpr Value atomic_unit_flux_density = 2.35051757077e5;      // T

    // — Atomic unit of current
    static constexpr Value atomic_unit_current = 6.6236182375082e-3;     // A

    // -------------------------------------------------------------
    // ATOMIC UNITS
    // -------------------------------------------------------------

    // — Atomic unit of mass
    static constexpr Value m_e = electron_mass; // kg

    // — Atomic unit of length (Bohr radius)
    static constexpr Value a0 = atomic_unit_length; // m

    // — Atomic unit of time
    static constexpr Value t_a = atomic_unit_time; // s

    // — Atomic unit of energy (Hartree energy)
    static constexpr Value Eh = hartree_energy_J; // J

    // — Atomic unit of electric field
    static constexpr Value E_h = atomic_unit_electric_field; // V/m

    // — Atomic unit of electric potential
    static constexpr Value V_h = hartree_energy_eV; // eV

    // — Atomic unit of current
    static constexpr Value I_h = atomic_unit_current; // A

    // — Atomic unit of magnetic moment
    static constexpr Value mu_h = bohr_magneton; // J/T

    // -------------------------------------------------------------
    // PLANCK UNITS
    // -------------------------------------------------------------

    // Planck length
    static constexpr Value l_p = 1.616255e-35; // m

    // Planck mass
    static constexpr Value m_p = 2.176434e-8; // kg

    // Planck time
    static constexpr Value t_p = 5.391247e-44; // s

    // Planck charge
    static constexpr Value q_p = 1.87554595641e-18; // C

    // Planck temperature
    static constexpr Value T_p = 1.416784e32; // K

    // Planck energy
    static constexpr Value E_p = 1.9561e9; // J

    // -------------------------------------------------------------
    // ASTRONOMICAL CONSTANTS
    // -------------------------------------------------------------

    // Astronomical unit
    static constexpr Value au = 1.495978707e11; // m

    // Light-year
    static constexpr Value ly = 9.4607304725808e15; // m

    // Parsec
    static constexpr Value pc = 3.08567758149137e16; // m

    // Solar mass
    static constexpr Value M_sun = 1.98847e30; // kg

    // Earth mass
    static constexpr Value M_earth = 5.9722e24; // kg

    // Jupiter mass
    static constexpr Value M_jupiter = 1.89813e27; // kg

    // Sun radius
    static constexpr Value R_sun = 6.96342e8; // m

    // Earth radius
    static constexpr Value R_earth = 6.3781e6; // m

    // Gravitational constant
    static constexpr Value G = 6.67430e-11; // m^3 kg^-1 s^-2

    // Standard gravity
    static constexpr Value g0 = 9.80665; // m/s^2

    // -------------------------------------------------------------
    // MASS, CHARGE, ATOMIC UNITS (SI)
    // -------------------------------------------------------------

    // Speed of light in vacuum (exact)
    static constexpr Value c = 299792458.0; // m/s

    // Magnetic constant, permeab­ility of free space, vacuum permeability (exactly defined)
    static constexpr Value mu0 = 1.25663706127e-6; // N/A^2

    // Eelectric constant, permit­ivitty of free space, vacuum permitivitty = 1/(?0 c^2)
    static constexpr Value epsilon0 = 8.8541878188e-12; // F/m

    // Planck constant (for energy-frequency relation)
    static constexpr Value h = 6.62607015e-34;

    // Reduced Planck constant
    static constexpr Value hbar = atomic_unit_action_Js;

    // Elementary charge (exact) - Atomic unit of charge
    static constexpr Value e = atomic_unit_charge; // C

    // Avogadro constant (exact)
    static constexpr Value NA = 6.02214076e23; // mol^-1

    // Boltzmann constant (exact)
    static constexpr Value kB = 1.380649e-23; // J/K

    // Faraday constant
    static constexpr Value F = 9.64853321233100184e4; // C/mol

    // Molar gas constant
    static constexpr Value R = 8.31446261815324; // J mol^-1 K^-1

    // Stefan–Boltzmann constant
    static constexpr Value sigma = 5.670374419e-8; // W/m^2 K^4

    // Rydberg constant
    static constexpr Value R_inf = 10973731.568160; // m^-1

    // Fine-structure constant ?
    static constexpr Value alpha = 0.00729735256815700; // 7.2973525693e-3

    // Conductance quantum G0 = 2e?/h
    static constexpr Value G0 = 7.748091729e-5; // S

    // Von Klitzing constant RK = h/e?
    static constexpr Value RK = 25812.80745; // Ohm

    // Josephson constant KJ = 2e/h
    static constexpr Value KJ = 483597.8484e9; // Hz/V

    // Magnetic flux quantum ?0 = h/(2e)
    static constexpr Value phi0 = 2.067833848e-15;

    // — Standard atmosphere
    static constexpr Value atm = 101325.0; // Pa

    // — Unified atomic mass unit (u)
    static constexpr Value u = atomic_mass_unit; // kg

    // Electron mass
    static constexpr Value me = electron_mass; // kg
    static constexpr Value me_u = 5.485799090441e-4; // atomic mass unit
    static constexpr Value me_MeV = electron_mass_energy_MeV; // MeV

    // Proton mass
    static constexpr Value mp = proton_mass; // kg
    static constexpr Value mp_u = 1.0072764665789; // u
    static constexpr Value mp_MeV = proton_mass_energy_MeV; // MeV

    // Neutron mass
    static constexpr Value mn = neutron_mass; // kg
    static constexpr Value mn_u = 1.00866491606; // u
    static constexpr Value mn_MeV = neutron_mass_energy_MeV; // MeV

    // Deuteron mass
    static constexpr Value md = deuteron_mass; // kg
    static constexpr Value md_u = 2.013553212544; // u
    static constexpr Value md_MeV = deuteron_mass_energy_MeV; // MeV

    // Alpha particle mass
    static constexpr Value malpha = alpha_mass; // kg
    static constexpr Value malpha_u = 4.001506179129; // u
    static constexpr Value malpha_MeV = alpha_mass_energy_MeV; // MeV

    // Helion (3He nucleus) mass
    static constexpr Value mh = helion_mass; // kg
    static constexpr Value mh_u = 3.014932246932; // u
    static constexpr Value mh_MeV = 2808.39161112; // MeV

    // Atomic mass constant
    static constexpr Value mu = atomic_mass_unit; // kg

    // Electron-proton mass ratio
    static constexpr Value me_mp = 5.446170214889e-4;

    // Electron-muon mass ratio
    static constexpr Value me_mm = 4.83633170e-3;

    // Neutron-proton mass ratio
    static constexpr Value mn_mp = 1.00137841946;

    // Deuteron-proton mass ratio
    static constexpr Value md_mp = 1.9990075012699;

    // Bohr magneton
    static constexpr Value muB = bohr_magneton; // J T^-1

    // Nuclear magneton
    static constexpr Value muN = nuclear_magneton; // J T^-1

    // Electron magnetic moment
    static constexpr Value mue = electron_magnetic_moment; // J T^-1

    // Proton magnetic moment
    static constexpr Value mup = proton_magnetic_moment; // J T^-1

    // Neutron magnetic moment
    static constexpr Value mun = neutron_magnetic_moment; // J T^-1

    // Deuteron magnetic moment
    static constexpr Value mud = deuteron_magnetic_moment; // J T^-1

    // Alpha particle magnetic moment
    static constexpr Value mualpha = alpha_magnetic_moment;

    // Reduced Compton wavelength of electron
    static constexpr Value lambda_bar_e = reduced_compton_wavelength; // m

    // Compton wavelength of electron
    static constexpr Value lambda_e = electron_compton_wavelength; // m

    // Classical electron radius
    static constexpr Value re = classical_electron_radius; // m

    // Thomson cross section
    static constexpr Value sigmae = 6.6524587051e-29; // m^2
}

#endif // CODATA2022_HPP