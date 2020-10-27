{
    volScalarField& he = thermo.he();

    //- ADDED CODE
    
    heatFluxCorr = dimensionedVector("heatFluxCorr", thermo.he().dimensions()/dimLength, Foam::vector(0,0,0));

    forAll(Y,i)
    {
        gradY = fvc::grad(Y[i]);
        
        forAll(mesh.cells(),celli)
        {
          // Note that composition.Hs() returns a dimensionless scalar 
          // of sensible enthalpy, therefore setting variable through value() function

          Hs.value() = composition.Hs(i,p[celli],T[celli]);
          heatFluxCorr[celli] += (1/LewisNumber[i] - 1)*Hs.value()*gradY[celli];
        }

    }

    fvScalarMatrix EEqn
    (
        fvm::ddt(rho, he) + mvConvection->fvmDiv(phi, he)
      + fvc::ddt(rho, K) + fvc::div(phi, K)
      + (
            he.name() == "e"
          ? fvc::div
            (
                fvc::absolute(phi/fvc::interpolate(rho), U),
                p,
                "div(phiv,p)"
            )
          : -dpdt
        )
      - fvm::laplacian(turbulence->alphaEff(), he)
      - fvc::div(turbulence->alphaEff()*heatFluxCorr,"div(heatFluxCorr)")  // New term
     ==
        reaction->Qdot()
      + fvOptions(rho, he)
    );

    EEqn.relax();

    fvOptions.constrain(EEqn);

    EEqn.solve();

    fvOptions.correct(he);

    thermo.correct();

    Info<< "min/max(T) = "
        << min(T).value() << ", " << max(T).value() << endl;
}
