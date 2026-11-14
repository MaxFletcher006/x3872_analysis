# X(3872) Resonance Analysis with LHCb Data

This repository contains the analysis workflow for studying the
**X(3872) resonance** using LHCb data.

The analysis reconstructs the decay chain:

``` text
B+ -> X(3872) K+
X(3872) -> J/psi pi+ pi-
J/psi -> mu+ mu-
```

The project covers event selection and candidate reconstruction with the
Gaudi/DaVinci framework, kinematic calculations using four-vectors,
mass-distribution studies with ROOT/RooFit, and signal-background
classification with TMVA.

## Physics Motivation

The X(3872) is a resonance in the charmonium-mass region. Studying its
mass distribution and production through B meson decays provides a way
to investigate its properties.

The analysis uses reconstructed decay products to form the J/psi,
X(3872), and B+ candidates. The invariant mass of a system is calculated
from the sum of the daughter particles' four-momenta.

## Invariant Mass Calculation

The invariant mass is calculated from the four-momenta of the daughter
particles:

``` text
M^2 = (sum of E_i)^2 - |sum of p_i|^2
```

Here, natural units are used, with `c = 1`.

## Decay Channel

The target decay chain is:

``` text
B+ -> K+ X(3872)
         |
         +-> J/psi pi+ pi-
                  |
                  +-> mu+ mu-
```

The reconstructed final-state particles are:

-   One kaon (`K+`).
-   Two pions (`pi+`, `pi-`).
-   Two muons (`mu+`, `mu-`).

## Data Sample and Selection

The project presentation describes the use of LHC Run 2 data-taking
periods:

-   2016
-   2017
-   2018

The selection workflow described in the presentation includes:

1.  Select muons and reconstruct `J/psi -> mu+ mu-`.
2.  Use RICH particle-identification information to distinguish kaons
    and pions.
3.  Reconstruct the `X(3872) -> J/psi pi+ pi-` candidate.
4.  Combine the X(3872) candidate with a kaon to reconstruct the B+
    candidate.
5.  Apply vertex, track-quality, mass, and kinematic requirements.
6.  Store selected candidates in a `DecayTreeTuple` for subsequent
    analysis.

The presentation lists example requirements including a J/psi mass
window of 3040--3140 MeV, an X(3872) candidate mass below 4000 MeV, and
a B+ candidate mass window of 5050--5450 MeV. These are the values
documented in the presentation; check the actual DaVinci configuration
before treating them as the final selection.

## Analysis Workflow

### 1. Candidate Reconstruction with DaVinci

The reconstruction and selection are implemented using the Gaudi/DaVinci
framework.

The documented workflow uses:

-   Particle-selection algorithms for muons, pions, and kaons.
-   `FilterDesktop` for applying particle-level selection requirements.
-   `CombineParticles` to build the J/psi, X(3872), and B+ candidates.
-   `DecayTreeTuple` to write selected candidate information into a ROOT
    tuple.

The tuple provides the variables needed for later kinematic studies and
mass fits.

### 2. Four-Vector and Invariant-Mass Calculation

A particle's four-momentum is represented as:

``` text
p = (E, px, py, pz)
```

For a reconstructed system, the four-momenta of its daughter particles
are added. The invariant mass is then obtained from:

``` text
M = sqrt(E_total^2 - px_total^2 - py_total^2 - pz_total^2)
```

This provides a way to calculate the mass of composite candidates from
their reconstructed decay products.

### 3. ROOT and RooFit Mass Fits

ROOT is used to inspect and process the reconstructed candidate data.
RooFit is used to model and fit the mass distributions.

The B-meson mass-fit macro shared during development uses:

-   Observable: `B_Jpsi_B_M`
-   Fit interval: 5200--5350 MeV/c\^2
-   Signal model: double-sided Crystal Ball (`RooCrystalBall`)
-   Background model: exponential (`RooExponential`)
-   Combined model: signal plus background (`RooAddPdf`)
-   Fit mode: extended maximum-likelihood fit

The macro also produces a mass-distribution plot, overlays the total
model and its signal/background components, and displays a pull
distribution.

A CSV input version can read `collision_data.csv` with the columns:

``` csv
B_Jpsi_B_M,X_Mass_DTF
5089.434566539821,3687.480382012117
5270.2013036084045,3779.1977610731637
```

For the B-mass fit, `B_Jpsi_B_M` is the fitted observable. `X_Mass_DTF`
is available in the same tuple but is not used by that particular
one-dimensional fit.

**Important:** The 5200--5350 MeV/c\^2 interval is the B-candidate fit
range, not the X(3872) mass range. The exact X(3872) fit range and model
should be specified in the corresponding X-mass fit macro.

### 4. Signal and Background Classification with TMVA

The project presentation also describes a TMVA classification study
using a Boosted Decision Tree (BDT).

The documented training setup is:

-   Signal sample: Monte Carlo simulation
-   Background sample: collision data
-   Classifier: BDT

Variables shown in the presentation include:

-   `B_Jpsi_Chi2NDoF`
-   `B_VerChi2NDoF`
-   `sum_hadron_pT`
-   `Min_Mu1_IPchi2_Mu2_IPchi2`
-   `Max_Mu1_IPchi2_Mu2_IPchi2`
-   `Min_Pi1_IPchi2_Pi2_IPchi2`
-   `Max_Pi1_IPchi2_Pi2_IPchi2`
-   `B_FDchi2`
-   `B_DIRA`
-   `B_IPChi2`
-   `K_IPChi2`

The presentation illustrates a BDT-score selection around `-0.1`, with
lower scores assigned to background-like candidates and higher scores
assigned to signal-like candidates. This threshold should be treated as
presentation-specific until confirmed against the final training and
validation configuration.

## Software and Tools

  Tool      Role
  --------- ------------------------------------------------
  Gaudi     LHCb software framework
  DaVinci   Event processing and candidate selection
  ROOT      Data storage, inspection, and analysis
  RooFit    Probability-density modelling and mass fitting
  TMVA      Multivariate signal-background classification
  C++       ROOT macros and analysis code

## Input Data

The CSV-based mass-fit macro expects `collision_data.csv` in the working
directory, with a header row containing at least:

-   `B_Jpsi_B_M`
-   `X_Mass_DTF`

Keep the units consistent with the analysis: the mass values shown in
the examples are in MeV/c\^2.

The original reconstruction workflow described in the presentation
writes selected candidates to a ROOT tuple. CSV files can be used as a
convenient input for standalone RooFit studies, provided the exported
columns and units are correct.

## Running the ROOT Macro

If ROOT and RooFit are available in the environment, and `lhcbStyle.C`
plus `collision_data.csv` are in the current working directory, run the
macro with:

``` bash
root -l b_m_full.C
```

For an interactive session:

``` bash
root -l
```

Then, from the ROOT prompt:

``` cpp
.L b_m_full.C
b_m_full()
```

Adjust the macro filename and function name if they differ in your local
repository.

## Repository Layout

The exact repository layout was not included in the supplied material. A
possible organization is:

``` text
.
├── README.md
├── data/
│   └── collision_data.csv
├── macros/
│   ├── b_m_full.C      # B-candidate mass fit
│   └──                 # X(3872) mass-fit macros
├── tmva/
│   └──                 # BDT training and evaluation
└── plots/
    └──                 # Analysis plots
```

Update this tree to match the actual files in the repository. Do not
commit private, restricted, or collaboration-controlled LHCb data to a
public repository.

## Current Scope and Next Steps

The material supplied for this README documents the reconstruction
chain, Run 2 data periods, invariant-mass calculation, B-mass RooFit
model, and a TMVA BDT study. It does not establish the final X(3872) fit
configuration, final numerical fit results, systematic uncertainties, or
a finalized BDT performance measurement.

Useful next steps for completing the analysis documentation are:

-   Document the exact DaVinci version, input data sample, and selection
    configuration.
-   Describe the definition and calibration of `X_Mass_DTF`.
-   Add the X(3872) mass-fit observable, fit interval, signal and
    background PDFs.
-   Record fit results with uncertainties and fit-quality information.
-   Document the TMVA training/test split, input configuration,
    validation, and chosen BDT working point.
-   Add plots and reproducible commands for each analysis stage.

## Acknowledgement

This work was carried out as part of an internship at the Institute of
Physics and Technology, Mongolian Academy of Sciences, in connection
with the LHCb experiment.

**Supervisor:** Dr. B. Baasansuren\
**Intern:** Bayarjavkhlan Byambadorj
