#ifndef CODATA2022_HPP
#define CODATA2022_HPP

/*
 * Copyright 2025-present Elle Solomina, a.rainman on gmail point com
 */

/** CODATA 2022 – Fundamental Physical Constants-- - Complete Listing
 * From: https://physics.nist.gov/constants NIST SP 961 (May 2024)
 * */

// -------------------------------------------------------------
// PHYSICAL CONSTANTS (SI)
// -------------------------------------------------------------
namespace codata2022 {
// -------------------------------------------------------------
// NUCLEAR / ISOTOPE CONSTANTS
// -------------------------------------------------------------

// Deuteron
constexpr double deuteron_mass = 3.3435837768e-27;               // kg
constexpr double deuteron_mass_energy_J = 3.00506323491e-10;     // J
constexpr double deuteron_mass_energy_MeV = 1875.61294500;       // MeV
constexpr double deuteron_g_factor = 0.8574382338;               // dimensionless
constexpr double deuteron_magnetic_moment = 4.33073548e-27;      // J/T
constexpr double deuteron_compton_wavelength = 6.6260700396e-16; // m

// Triton
constexpr double triton_mass = 5.0073567512e-27;
constexpr double triton_mass_energy_J = 4.5003878119e-10;
constexpr double triton_mass_energy_MeV = 2808.92113668;
constexpr double triton_g_factor = 5.957924895;
constexpr double triton_magnetic_moment = 2.97896244e-26;
constexpr double triton_compton_wavelength = 4.7592429830e-16; // m

// Helion (3He nucleus)
constexpr double helion_mass = 5.0064127862e-27;
constexpr double helion_mass_energy_J = 4.4995394185e-10;
constexpr double helion_mass_energy_MeV = 2808.39161112;
constexpr double helion_g_factor = -4.2552506995;
constexpr double helion_magnetic_moment = -1.07461755198e-26;
constexpr double helion_compton_wavelength = 4.4029672189e-16; // m

// Alpha particle (4He nucleus)
constexpr double alpha_mass = 6.6446573450e-27;
constexpr double alpha_mass_energy_J = 5.9719201997e-10;
constexpr double alpha_mass_energy_MeV = 3727.3794118;
constexpr double alpha_g_factor = 0.0;
constexpr double alpha_magnetic_moment = 0.0; // spin-0, zero magnetic moment
constexpr double alpha_compton_wavelength = 3.3233516020e-16; // m

// Nuclear magneton
constexpr double nuclear_magneton = 5.0507837393e-27; // J/T

// -------------------------------------------------------------
// PARTICLE MASSES, MAGNETIC MOMENTS, COMPTON WAVELENGTHS, ETC.
// -------------------------------------------------------------

// — Masses (kg)
constexpr double electron_mass = 9.1093837139e-31;
constexpr double proton_mass = 1.67262192595e-27;
constexpr double neutron_mass = 1.67492750056e-27;
// constexpr double deuteron_mass = ;
// constexpr double triton_mass = ;
constexpr double alpha_particle_mass = alpha_mass;

// — Mass energy equivalents (J)
constexpr double electron_mass_energy_J = 8.1871057880e-14;
constexpr double proton_mass_energy_J = 1.50327761802e-10;
constexpr double neutron_mass_energy_J = 1.50534976514e-10;
// constexpr double deuteron_mass_energy_J = ;
// constexpr double triton_mass_energy_J = ;
constexpr double alpha_particle_mass_energy_J = 5.9719201997e-10;

// — Mass energy equivalents (MeV)
constexpr double electron_mass_energy_MeV = 0.51099895069;
constexpr double proton_mass_energy_MeV = 938.27208943;
constexpr double neutron_mass_energy_MeV = 939.56542194;
// constexpr double deuteron_mass_energy_MeV = ;
// constexpr double triton_mass_energy_MeV = ;
constexpr double alpha_particle_mass_energy_MeV = alpha_mass_energy_MeV;

// — Mass ratios
constexpr double proton_electron_mass_ratio = 1836.152673426;
constexpr double neutron_proton_mass_ratio = 1.00137841946;
constexpr double deuteron_electron_mass_ratio = 3670.482967655;
constexpr double alpha_electron_mass_ratio = 7294.29954171;

// — Classical radius & Compton wavelengths
constexpr double classical_electron_radius = 2.8179403205e-15;    // m
constexpr double electron_compton_wavelength = 2.42631023538e-12; // m
constexpr double proton_compton_wavelength = 1.32140985360e-15;   // m
constexpr double neutron_compton_wavelength = 1.31959090382e-15;  // m
constexpr double reduced_compton_wavelength = 3.8615926744e-13;   // m

// — Magnetic moments / gyromagnetic ratios
constexpr double bohr_magneton = 9.2740100657e-24; // J T^-1
// constexpr double nuclear_magneton = ;   // J T^-1
constexpr double electron_magnetic_moment = -9.2847646917e-24; // J T^-1
constexpr double proton_magnetic_moment = 1.41060679545e-26;   // J T^-1
constexpr double neutron_magnetic_moment = -9.6623653e-27;     // J T^-1

// — Gyromagnetic ratios (s^-1 T^-1)
constexpr double electron_gyro_ratio = 1.76085962784e11;
constexpr double proton_gyro_ratio = 2.6752218708e8;
constexpr double neutron_gyro_ratio = 1.83247174e8;

// — g-factors (dimensionless)
constexpr double electron_g_factor = -2.00231930436092;
constexpr double proton_g_factor = 5.5856946893;
constexpr double neutron_g_factor = -3.82608552;

// — Magnetic moment to Bohr / nuclear magneton ratios
constexpr double proton_to_bohr_moment_ratio = 1.52103220230e-3;
constexpr double neutron_to_bohr_moment_ratio = -1.04187565e-3;

// — Mass differences
constexpr double neutron_proton_mass_difference = 2.30557461e-30;    // kg
constexpr double neutron_proton_mass_diff_energy_J = 2.07214712e-13; // J
constexpr double neutron_proton_mass_diff_energy_MeV = 1.29333251;   // MeV

// -------------------------------------------------------------
// ATOMIC / ATOMIC-UNIT CONSTANTS
// -------------------------------------------------------------

// — Atomic mass unit
constexpr double atomic_mass_unit = 1.66053906892e-27; // kg

// — Bohr radius
constexpr double bohr_radius = 5.29177210544e-11; // m

// — Hartree energy (atomic unit of energy)
constexpr double hartree_energy_J = 4.3597447222060e-18; // J
constexpr double hartree_energy_eV = 27.211386245981;    // eV

// — Atomic unit of length, mass, time, velocity
constexpr double atomic_unit_length = bohr_radius;       // m (a0)
constexpr double atomic_unit_mass = electron_mass;       // kg (electron mass)
constexpr double atomic_unit_time = 2.4188843265864e-17; // s
constexpr double atomic_unit_velocity = 2.18769126216e6; // m s^-1

// — Atomic unit of energy / action / momentum
constexpr double atomic_unit_energy_J = hartree_energy_J;          // J
constexpr double reduced_planck_constant = 1.0545718176461565e-34; // J s (?)
constexpr double atomic_unit_momentum = 1.99285191545e-24;         // kg m s^-1

// — Atomic unit of charge (same as elementary charge)
constexpr double atomic_unit_charge = 1.602176634e-19; // C

// — Atomic unit of electric dipole moment
constexpr double atomic_unit_dipole_moment = 8.4783536198e-30; // C m

// — Atomic unit of electric field
constexpr double atomic_unit_electric_field = 5.14220675112e11; // V m^-1

// — Atomic unit of electric potential (Hartree / e)
constexpr double atomic_unit_potential = hartree_energy_eV; // V

// — Atomic unit of force
constexpr double atomic_unit_force = 8.2387235038e-8; // N

// — Atomic unit of electric polarizability
constexpr double atomic_unit_polarizability = 1.64877727212e-41; // C^2 m^2 J^-1

// — Atomic unit of magnetic dipole moment
constexpr double atomic_unit_magnetic_dipole = 1.85480201315e-23; // J T^-1

// — Atomic unit of magnetic flux density
constexpr double atomic_unit_flux_density = 2.35051757077e5; // T

// — Atomic unit of current
constexpr double atomic_unit_current = 6.6236182375082e-3; // A

// -------------------------------------------------------------
// ATOMIC UNITS
// -------------------------------------------------------------

// — Atomic unit of mass
constexpr double m_e = electron_mass; // kg

// — Atomic unit of length (Bohr radius)
constexpr double a0 = atomic_unit_length; // m

// — Atomic unit of time
constexpr double t_a = atomic_unit_time; // s

// — Atomic unit of energy (Hartree energy)
constexpr double Eh = hartree_energy_J; // J

// — Atomic unit of electric field
constexpr double E_h = atomic_unit_electric_field; // V/m

// — Atomic unit of electric potential
constexpr double V_h = hartree_energy_eV; // eV

// — Atomic unit of current
constexpr double I_h = atomic_unit_current; // A

// — Atomic unit of magnetic moment
constexpr double mu_h = bohr_magneton; // J/T

// -------------------------------------------------------------
// PLANCK UNITS
// -------------------------------------------------------------

// Planck length
constexpr double l_P = 1.616255e-35; // m

// Planck mass
constexpr double m_P = 2.176434e-8; // kg

// Planck mass energy equivalent in GeV
// https://physics.nist.gov/cgi-bin/cuu/value?plkmc2gev
constexpr double m_P_GeV = 1.220890e-19; // GeV

// Planck time
// https://physics.nist.gov/cgi-bin/cuu/value?plkt
constexpr double t_P = 5.391247e-44; // s

// Planck charge
constexpr double q_P = 1.87554595641e-18; // C

// Planck temperature
// https://physics.nist.gov/cgi-bin/cuu/value?plktmp
constexpr double T_P = 1.416784e32; // K

// Planck energy
constexpr double E_P = 1.9561e9; // J

// -------------------------------------------------------------
// ASTRONOMICAL CONSTANTS
// -------------------------------------------------------------

// Astronomical unit
constexpr double au = 1.495978707e11; // m

// Light-year
constexpr double ly = 9.4607304725808e15; // m

// Parsec
constexpr double pc = 3.08567758149137e16; // m

// Solar (Sun)
constexpr double M_sun = 1.98847e30; // kg
constexpr double R_sun = 6.96342e8;  // m

// Mercury
constexpr double M_mercury = 3.3011e23; // kg
constexpr double R_mercury = 2.4397e6;  // m

// Venus
constexpr double M_venus = 4.8675e24; // kg
constexpr double R_venus = 6.0518e6;  // m

// Earth (Terra)
constexpr double M_earth = 5.9722e24; // kg
constexpr double R_earth = 6.3781e6;  // m

// Moon (Luna) Earth moon
constexpr double M_moon = 7.342e22; // kg
constexpr double R_moon = 1.7374e6; // m

// Mars
constexpr double M_mars = 6.4171e23; // kg
constexpr double R_mars = 3.3895e6;  // m

// Jupiter
constexpr double M_jupiter = 1.89813e27; // kg
constexpr double R_jupiter = 69911000;   // m

// Saturn
constexpr double M_saturn = 5.6834e26; // kg
constexpr double R_saturn = 5.8232e7;  // m

// Uranus
constexpr double M_uranus = 8.6810e25; // kg
constexpr double R_uranus = 2.5362e7;  // m

// Neptune
constexpr double M_neptune = 1.0241e26; // kg
constexpr double R_neptune = 2.4622e7;  // m

// Pluto (Dwarf Planet)
constexpr double M_pluto = 1.303e22; // kg
constexpr double R_pluto = 1.1883e6; // m

// Gravitational constant
// https://physics.nist.gov/cgi-bin/cuu/value?bg
constexpr double G = 6.67430e-11; // m^3 kg^-1 s^-2

// Standard gravity
constexpr double g0 = 9.80665; // m/s^2

// -------------------------------------------------------------
// MASS, CHARGE, ATOMIC UNITS (SI)
// -------------------------------------------------------------

// Speed of light in vacuum (exact)
constexpr double c = 299792458.0; // m/s

// Magnetic constant, permeab­ility of free space, vacuum permeability
constexpr double mu0 = 1.25663706126960e-6; // N/A^2

// Electric constant, permittivity of free space, vacuum permeability
constexpr double epsilon0 = 8.85418781881409e-12; // F/m

// Characteristic impedance of vacuum
// https://physics.nist.gov/cgi-bin/cuu/value?z0
constexpr double Z0 = 376.730313412; // Ohm

// Planck constant (for energy-frequency relation)
constexpr double h = 6.62607015e-34; // J*Hz^-1

// Reduced Planck constant
constexpr double hbar = reduced_planck_constant;

// Elementary charge (exact) - Atomic unit of charge
constexpr double e = atomic_unit_charge; // C

// Avogadro constant (exact)
constexpr double NA = 6.02214076e23; // mol^-1

// Boltzmann constant (exact)
constexpr double kB = 1.380649e-23; // J/K

// Faraday constant
constexpr double F = 9.64853321233100184e4; // C/mol

// Molar gas constant
constexpr double R = 8.31446261815324; // J mol^-1 K^-1

// Stefan–Boltzmann constant
constexpr double sigma = 5.670374419e-8; // W/m^2 K^4

// Rydberg constant
constexpr double R_inf = 10973731.568160; // m^-1

// Fine-structure constant ?
constexpr double alpha = 7.2973525643e-3;

// Conductance quantum G0 = 2e?/h
constexpr double G0 = 7.748091729e-5; // S

// Von Klitzing constant RK = h/e?
constexpr double RK = 25812.80745; // Ohm

// Josephson constant KJ = 2e/h
constexpr double KJ = 483597.8484e9; // Hz/V

// Magnetic flux quantum ?0 = h/(2e)
constexpr double phi0 = 2.067833848e-15;

// Standard atmosphere
constexpr double atm = 101325.0; // Pa

// Unified atomic mass unit (u)
constexpr double u = atomic_mass_unit; // kg

// Electron mass
constexpr double me = electron_mass;                // kg
constexpr double me_u = 5.485799090441e-4;          // u
constexpr double me_MeV = electron_mass_energy_MeV; // MeV

// Proton mass
constexpr double mp = proton_mass;                // kg
constexpr double mp_u = 1.0072764665789;          // u
constexpr double mp_MeV = proton_mass_energy_MeV; // MeV

// Neutron mass
constexpr double mn = neutron_mass;                // kg
constexpr double mn_u = 1.00866491606;             // u
constexpr double mn_MeV = neutron_mass_energy_MeV; // MeV

// Deuteron mass
constexpr double md = deuteron_mass;                // kg
constexpr double md_u = 2.013553212544;             // u
constexpr double md_MeV = deuteron_mass_energy_MeV; // MeV

// Alpha particle mass
constexpr double malpha = alpha_mass;                // kg
constexpr double malpha_u = 4.001506179129;          // u
constexpr double malpha_MeV = alpha_mass_energy_MeV; // MeV

// Helion (3He nucleus) mass
constexpr double mh = helion_mass;                // kg
constexpr double mh_u = 3.014932246932;           // u
constexpr double mh_MeV = helion_mass_energy_MeV; // MeV

// Atomic mass constant
constexpr double mu = atomic_mass_unit; // kg

// Electron-proton mass ratio
constexpr double me_mp = 5.446170214889e-4;

// Electron-muon mass ratio
constexpr double me_mm = 4.83633170e-3;

// Neutron-proton mass ratio
constexpr double mn_mp = 1.00137841946;

// Deuteron-proton mass ratio
constexpr double md_mp = 1.9990075012699;

// Bohr magneton
constexpr double muB = bohr_magneton; // J T^-1

// Nuclear magneton
constexpr double muN = nuclear_magneton; // J T^-1

// Electron magnetic moment
constexpr double mue = electron_magnetic_moment; // J T^-1

// Proton magnetic moment
constexpr double mup = proton_magnetic_moment; // J T^-1

// Neutron magnetic moment
constexpr double mun = neutron_magnetic_moment; // J T^-1

// Deuteron magnetic moment
constexpr double mud = deuteron_magnetic_moment; // J T^-1

// Alpha particle magnetic moment
constexpr double mualpha = alpha_magnetic_moment; // J T^-1

// Reduced Compton wavelength of electron
constexpr double lambda_bar_e = reduced_compton_wavelength; // m

// Compton wavelength of electron
constexpr double lambda_e = electron_compton_wavelength; // m

// Classical electron radius
constexpr double re = classical_electron_radius; // m

// Thomson cross section
constexpr double sigmae = 6.6524587051e-29; // m^2

} // namespace codata2022

#endif // CODATA2022_HPP
