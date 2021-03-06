#include "createRDeltaT.H"

Info<< "Reading thermophysical properties\n" << endl;
autoPtr<psiReactionThermo> pThermo(psiReactionThermo::New(mesh));
psiReactionThermo& thermo = pThermo();
thermo.validate(args.executable(), "h", "e");

basicSpecieMixture& composition = thermo.composition();
PtrList<volScalarField>& Y = composition.Y();

const word inertSpecie(thermo.lookup("inertSpecie"));
if (!composition.species().found(inertSpecie))
{
    FatalIOErrorIn(args.executable().c_str(), thermo)
        << "Inert specie " << inertSpecie << " not found in available species "
        << composition.species() << exit(FatalIOError);
}

volScalarField rho
(
    IOobject
    (
        "rho",
        runTime.timeName(),
        mesh
    ),
    thermo.rho()
);

Info<< "Reading field U\n" << endl;
volVectorField U
(
    IOobject
    (
        "U",
        runTime.timeName(),
        mesh,
        IOobject::MUST_READ,
        IOobject::AUTO_WRITE
    ),
    mesh
);

volScalarField& p = thermo.p();

#include "compressibleCreatePhi.H"

pressureControl pressureControl(p, rho, pimple.dict(), false);

mesh.setFluxRequired(p.name());

Info << "Creating turbulence model.\n" << nl;
autoPtr<compressible::momentumTransportModel> turbulence
(
    compressible::momentumTransportModel::New
    (
        rho,
        U,
        phi,
        thermo
    )
);

Info<< "Creating thermophysical transport model\n" << endl;
autoPtr<psiReactionThermophysicalTransportModel> thermophysicalTransport
(
    psiReactionThermophysicalTransportModel::New(turbulence(), thermo)
);

Info<< "Creating reaction model\n" << endl;
autoPtr<CombustionModel<psiReactionThermo>> reaction
(
    CombustionModel<psiReactionThermo>::New(thermo, turbulence())
);

Info<< "Creating field dpdt\n" << endl;
volScalarField dpdt
(
    IOobject
    (
        "dpdt",
        runTime.timeName(),
        mesh
    ),
    mesh,
    dimensionedScalar(p.dimensions()/dimTime, 0)
);

Info<< "Creating field kinetic energy K\n" << endl;
volScalarField K("K", 0.5*magSqr(U));


multivariateSurfaceInterpolationScheme<scalar>::fieldTable fields;

forAll(Y, i)
{
    fields.add(Y[i]);
}
fields.add(thermo.he());


//- ADDED CODE

volVectorField heatFluxCorr
(
    IOobject
    (
        "heatFluxCorr",
        runTime.timeName(),
        mesh,
        IOobject::NO_READ,
        IOobject::NO_WRITE
    ),
    mesh,
    dimensionedVector("heatFluxCorr", thermo.he().dimensions()/dimLength, Foam::vector(0,0,0))
);

volVectorField gradY
(
    IOobject
    (
        "gradY",
        runTime.timeName(),
        mesh,
        IOobject::NO_READ,
        IOobject::NO_WRITE
    ),
    mesh,
    dimensionedVector("gradY", dimless/dimLength, Foam::vector(0,0,0))
);


dimensionedScalar Hs("Hs",thermo.he().dimensions(),0.0);
label Ysize = Y.size();

scalarList LewisNumber(Ysize,1.0);

Info<< "\nReading Lewis numbers\n" << endl;
IOdictionary chemistryProperties
(
    IOobject
    (
        "chemistryProperties",    
        runTime.constant(),     
        mesh,                   
        IOobject::MUST_READ,    
        IOobject::NO_WRITE      
    )
);


dictionary LewisNumberDict(chemistryProperties.subDict("LewisNumbers"));


for (label i=0; i<Ysize; i++)
{
    if (LewisNumberDict.found(Y[i].member()))
    {
        LewisNumber[i] = readScalar(LewisNumberDict.lookup(Y[i].member()));
    }
    Info<<"Lewis number of specie "<<Y[i].name()<<" is: "<<LewisNumber[i]<<endl;
}


#include "createMRF.H"
#include "createFvOptions.H"
