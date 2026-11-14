X(3872) Resonance Analysis with LHCb Data
This repository contains the analysis workflow for studying the
(X(3872)) resonance using LHCb data. The analysis reconstructs the
decay chain
\[
`\rightarrow `{=tex}J/`\psi`{=tex},`\pi`{=tex}^+`\pi`{=tex}^-,
`\qquad `{=tex}J/`\psi `{=tex}`\rightarrow `{=tex}`\mu`{=tex}^+`\mu`{=tex}^-.
\]
The project covers event selection and candidate reconstruction with the
Gaudi/DaVinci framework, kinematic calculations using four-vectors,
mass-distribution studies with ROOT/RooFit, and signal--background
classification with TMVA.
Status: This README documents the workflow described in the
project presentation and the ROOT/RooFit macro shared during
development. Exact dataset paths, production configuration, and final
fit results should be added when they are confirmed in the repository.

Physics motivation
The (X(3872)) is a resonance in the charmonium-mass region. Studying its
mass distribution and production through (B^+) decays provides a way to
investigate its properties.

The analysis uses reconstructed decay products to form the
(J/`\psi`{=tex}), (X(3872)), and (B^+) candidates. The invariant mass
of a system is calculated from the sum of the daughter particles'
four-momenta:
`\left`{=tex}\|`\sum`{=tex}\_i `\vec{p}`{=tex}\_i`\right`{=tex}\|\^2, \]

using natural units where (c=1).

## Decay channel

The target decay chain is:

``` text
B+ -> K+ X(3872)
         |
         +-> J/psi pi+ pi-
                  |
                  +-> mu+ mu-
```

The reconstructed final-state particles are:

-   one kaon ((K\^+));
-   two pions ((`\pi`{=tex}\^+), (`\pi`{=tex}\^-));
-   two muons ((`\mu`{=tex}\^+), (`\mu`{=tex}\^-)).

## Data sample and selection

The project presentation describes the use of LHC Run 2 data-taking
periods:

-   2016
-   2017
-   2018

The selection workflow described in the presentation includes:

1.  Select muons and reconstruct
    (J/`\psi `{=tex}`\rightarrow `{=tex}`\mu`{=tex}^+`\mu`{=tex}^-).
2.  Use RICH particle-identification information to distinguish kaons
    and pions.
3.  Reconstruct the (X(3872)
    `\rightarrow `{=tex}J/`\psi`{=tex}`\pi`{=tex}^+`\pi`{=tex}^-)
    candidate.
4.  Combine the (X(3872)) candidate with a kaon to reconstruct the
    (B\^+) candidate.
5.  Apply vertex, track-quality, mass, and kinematic requirements.
6.  Store selected candidates in a `DecayTreeTuple` for subsequent
    analysis.

The presentation lists example requirements including a (J/`\psi`{=tex})
mass window of 3040--3140 MeV, an (X(3872)) candidate mass below 4000
MeV, and a (B\^+) candidate mass window of 5050--5450 MeV. These are the
values documented in the presentation; check the actual DaVinci
configuration before treating them as the final selection.

## Analysis workflow

### 1. Candidate reconstruction with DaVinci

The reconstruction and selection are implemented using the Gaudi/DaVinci
framework.

The documented workflow uses:

-   particle-selection algorithms for muons, pions, and kaons;
-   `FilterDesktop` for applying particle-level selection requirements;
-   `CombineParticles` to build the (J/`\psi`{=tex}), (X(3872)), and
    (B\^+) candidates;
-   `DecayTreeTuple` to write selected candidate information into a ROOT
    tuple.

The tuple provides the variables needed for later kinematic studies and
mass fits.

### 2. Four-vector and invariant-mass calculation

A particle's four-momentum is represented as

\[ p\^`\mu `{=tex}= (E, p_x, p_y, p_z). \]

For a reconstructed system, the four-momenta of its daughter particles
are added. The invariant mass is then obtained from

\[ M = `\sqrt{E_{\mathrm{tot}}^2 -
          p_{x,\mathrm{tot}}^2 -
          p_{y,\mathrm{tot}}^2 -
          p_{z,\mathrm{tot}}^2}`{=tex}. \]

This provides a way to calculate the mass of composite candidates from
their reconstructed decay products.

### 3. ROOT and RooFit mass fits

ROOT is used to inspect and process the reconstructed candidate data.
RooFit is used to model and fit the mass distributions.

The B-meson mass-fit macro shared during development uses:

-   observable: `B_Jpsi_B_M`;
-   fit interval: 5200--5350 MeV/(c\^2);
-   signal model: double-sided Crystal Ball (`RooCrystalBall`);
-   background model: exponential (`RooExponential`);
-   combined model: signal plus background (`RooAddPdf`);
-   fit mode: extended maximum-likelihood fit.

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

**Important:** the 5200--5350 MeV/(c\^2) interval is the B-candidate fit
range, not the X(3872) mass range. The exact X(3872) fit range and model
should be specified in the corresponding X-mass fit macro.

### 4. Signal and background classification with TMVA

The project presentation also describes a TMVA classification study
using a Boosted Decision Tree (BDT).

The documented training setup is:

-   signal sample: Monte Carlo simulation;
-   background sample: collision data;
-   classifier: BDT.

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

The presentation illustrates a BDT-score selection around (-0.1), with
lower scores assigned to background-like candidates and higher scores to
signal-like candidates. This threshold should be treated as
presentation-specific until confirmed against the final training and
validation configuration.

## Software and tools

The documented analysis uses the following software:

  Tool      Role
  --------- ------------------------------------------------
  Gaudi     LHCb software framework
  DaVinci   Event processing and candidate selection
  ROOT      Data storage, inspection, and analysis
  RooFit    Probability-density modelling and mass fitting
  TMVA      Multivariate signal--background classification
  C++       ROOT macros and analysis code

## Input data

The CSV-based mass-fit macro expects `collision_data.csv` in the working
directory, with a header row containing at least:

-   `B_Jpsi_B_M`
-   `X_Mass_DTF`

Keep the units consistent with the analysis: the mass values shown in
the examples are in MeV/(c\^2).

The original reconstruction workflow described in the presentation
writes selected candidates to a ROOT tuple. CSV files can be used as a
convenient input for standalone RooFit studies, provided the exported
columns and units are correct.

## Running the ROOT macro

If ROOT and RooFit are available in the environment, and `lhcbStyle.C`
plus `collision_data.csv` are in the current working directory, run the
macro with:

``` bash
root -l -q 'b_m_full.C'
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

## Repository layout

The exact repository layout was not included in the supplied material. A
possible organization is:

``` text
.
├── README.md
├── data/
│   └── collision_data.csv
├── davinci/
│   └──                 # DaVinci configuration scripts
├── macros/
│   ├── b_m_full.C      # B-candidate mass fit
│   └──                 # X(3872) mass-fit macros
├── tmva/
│   └──                 # BDT training and evaluation
└── figures/
    └──                 # Analysis plots
```

Update this tree to match the actual files in the repository. Do not
commit private, restricted, or collaboration-controlled LHCb data to a
public repository.

## Current scope and next steps

The material supplied for this README documents the reconstruction
chain, Run 2 data periods, invariant-mass calculation, B-mass RooFit
model, and a TMVA BDT study. It does not establish the final X(3872) fit
configuration, final numerical fit results, systematic uncertainties, or
a finalized BDT performance measurement.

Useful next steps for completing the analysis documentation are:

-   document the exact DaVinci version, input data sample, and selection
    configuration;
-   describe the definition and calibration of `X_Mass_DTF`;
-   add the X(3872) mass-fit observable, fit interval, signal and
    background PDFs;
-   record fit results with uncertainties and fit-quality information;
-   document the TMVA training/test split, input configuration,
    validation, and chosen BDT working point;
-   add plots and reproducible commands for each analysis stage.

## Acknowledgement

This work was carried out as part of an internship at the Institute of
Physics and Technology, Mongolian Academy of Sciences, in connection
with the LHCb experiment.

**Supervisor:** Dr. B. Baasansuren\
**Intern:** Bayarjavkhlan Byambadorj
