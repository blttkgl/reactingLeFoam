# reactingLeFoam
![OpenFOAM 7](https://img.shields.io/badge/OpenFOAM-7-brightgreen.svg)

An OpenFOAM reacting solver which takes the Lewis number of given species as user input. Developed by Bulut Tekgül & Heikki Kahila, Aalto University, Finland.

* The fluxes for species mass fractions and enthalpy are given as:

![equation](https://latex.codecogs.com/png.latex?%5Csmall%20F_i%20%3D%20m%20Y_i%20-%20%5Cfrac%7B%5Clambda%7D%7BLe_i%20c_p%7D%5Cfrac%7B%5Cpartial%20Y_i%7D%7B%5Cpartial%20s%7D)

![equation](https://latex.codecogs.com/png.latex?%5Csmall%20F_h%20%3D%20mh%20-%20%5Cfrac%7B%5Clambda%7D%7Bc_p%7D%5Cfrac%7B%5Cpartial%20h%7D%7B%5Cpartial%20s%7D%20-%20%5Cfrac%7B%5Clambda%7D%7Bc_p%7D%5Csum_%7Bi%3D1%7D%5E%7BN_s%7D%20%5Cleft%20%28%5Cfrac%7B1%7D%7BLe_i%7D%20-%201%20%5Cright%20%29%20h_i%20%5Cfrac%7B%5Cpartial%20Y_i%7D%7B%5Cpartial%20s%7D)

where unity Lewis number assumption gives us the formulation exists within OpenFOAM, where species mass fraction flux is λ/c<sub>p</sub> and right hand side term in the enthalpy equation does not exist.

* This solver introduces this formulation back into species mass fraction and enthalpy equations. The Lewis number of a given specie is defined by the user. Undefined species automatically gets unity Lewis number.

* Note that current OpenFOAM-dev branch (as of October 2020) includes a file structure which supports a multi-component mixture model, so this solver may be completely obsolete by the next stable release of OpenFOAM.



