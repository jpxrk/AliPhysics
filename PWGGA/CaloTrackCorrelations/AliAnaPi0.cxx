/**************************************************************************
 * Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 *                                                                        *
 * Author: The ALICE Off-line Project.                                    *
 * Contributors are mentioned in the code where appropriate.              *
 *                                                                        *
 * Permission to use, copy, modify and distribute this software and its   *
 * documentation strictly for non-commercial purposes is hereby granted   *
 * without fee, provided that the above copyright notice appears in all   *
 * copies and that both the copyright notice and this permission notice   *
 * appear in the supporting documentation. The authors make no claims     *
 * about the suitability of this software for any purpose. It is          *
 * provided "as is" without express or implied warranty.                  *
 **************************************************************************/

//_________________________________________________________________________
// Class to collect two-photon invariant mass distributions for
// extracting raw pi0 yield.
// Input is produced by AliAnaPhoton (or any other analysis producing output AliAODPWG4Particles), 
// it will do nothing if executed alone
//
//-- Author: Dmitri Peressounko (RRC "KI") 
//-- Adapted to CaloTrackCorr frame by Lamia Benhabib (SUBATECH)
//-- and Gustavo Conesa (INFN-Frascati)
//_________________________________________________________________________


// --- ROOT system ---
#include "TH3.h"
#include "TH2F.h"
//#include "Riostream.h"
#include "TCanvas.h"
#include "TPad.h"
#include "TROOT.h"
#include "TClonesArray.h"
#include "TObjString.h"
#include "TDatabasePDG.h"

//---- AliRoot system ----
#include "AliAnaPi0.h"
#include "AliCaloTrackReader.h"
#include "AliCaloPID.h"
#include "AliStack.h"
#include "AliFiducialCut.h"
#include "TParticle.h"
#include "AliVEvent.h"
#include "AliESDCaloCluster.h"
#include "AliESDEvent.h"
#include "AliAODEvent.h"
#include "AliNeutralMesonSelection.h"
#include "AliMixedEvent.h"
#include "AliAODMCParticle.h"

// --- Detectors --- 
#include "AliPHOSGeoUtils.h"
#include "AliEMCALGeometry.h"

ClassImp(AliAnaPi0)

//______________________________________________________
AliAnaPi0::AliAnaPi0() : AliAnaCaloTrackCorrBaseClass(),
fEventsList(0x0), 
fCalorimeter(""),            fNModules(22),
fUseAngleCut(kFALSE),        fUseAngleEDepCut(kFALSE),     fAngleCut(0),                 fAngleMaxCut(7.),
fMultiCutAna(kFALSE),        fMultiCutAnaSim(kFALSE),
fNPtCuts(0),                 fNAsymCuts(0),                fNCellNCuts(0),               fNPIDBits(0),  
fMakeInvPtPlots(kFALSE),     fSameSM(kFALSE),              
fFillSMCombinations(kFALSE), fCheckConversion(kFALSE),
fFillBadDistHisto(kFALSE),   fFillSSCombinations(kFALSE),  
fFillAngleHisto(kFALSE),     fFillAsymmetryHisto(kFALSE),  fFillOriginHisto(0),          fFillArmenterosThetaStar(0),
fCheckAccInSector(kFALSE),
//Histograms
fhAverTotECluster(0),        fhAverTotECell(0),            fhAverTotECellvsCluster(0),
fhEDensityCluster(0),        fhEDensityCell(0),            fhEDensityCellvsCluster(0),
fhReMod(0x0),                fhReSameSideEMCALMod(0x0),    fhReSameSectorEMCALMod(0x0),  fhReDiffPHOSMod(0x0), 
fhMiMod(0x0),                fhMiSameSideEMCALMod(0x0),    fhMiSameSectorEMCALMod(0x0),  fhMiDiffPHOSMod(0x0),
fhReConv(0x0),               fhMiConv(0x0),                fhReConv2(0x0),  fhMiConv2(0x0),
fhRe1(0x0),                  fhMi1(0x0),                   fhRe2(0x0),                   fhMi2(0x0),      
fhRe3(0x0),                  fhMi3(0x0),                   fhReInvPt1(0x0),              fhMiInvPt1(0x0),  
fhReInvPt2(0x0),             fhMiInvPt2(0x0),              fhReInvPt3(0x0),              fhMiInvPt3(0x0),
fhRePtNCellAsymCuts(0x0),    fhMiPtNCellAsymCuts(0x0),     fhRePtNCellAsymCutsSM(),  
fhRePIDBits(0x0),            fhRePtMult(0x0),              fhReSS(), 
fhRePtAsym(0x0),             fhRePtAsymPi0(0x0),           fhRePtAsymEta(0x0),  
fhEventBin(0),               fhEventMixBin(0),
fhCentrality(0x0),           fhCentralityNoPair(0x0),
fhEventPlaneResolution(0x0),
fhRealOpeningAngle(0x0),     fhRealCosOpeningAngle(0x0),   fhMixedOpeningAngle(0x0),     fhMixedCosOpeningAngle(0x0),
// MC histograms
fhPrimPi0E(0x0),             fhPrimPi0Pt(0x0),
fhPrimPi0AccE(0x0),          fhPrimPi0AccPt(0x0),
fhPrimPi0Y(0x0),             fhPrimPi0AccY(0x0),
fhPrimPi0Yeta(0x0),          fhPrimPi0YetaYcut(0x0),       fhPrimPi0AccYeta(0x0),
fhPrimPi0Phi(0x0),           fhPrimPi0AccPhi(0x0),
fhPrimPi0OpeningAngle(0x0),  fhPrimPi0OpeningAngleAsym(0x0),fhPrimPi0CosOpeningAngle(0x0),
fhPrimPi0PtCentrality(0),    fhPrimPi0PtEventPlane(0),
fhPrimPi0AccPtCentrality(0), fhPrimPi0AccPtEventPlane(0),
fhPrimEtaE(0x0),             fhPrimEtaPt(0x0),             
fhPrimEtaAccE(0x0),          fhPrimEtaAccPt(0x0),
fhPrimEtaY(0x0),             fhPrimEtaAccY(0x0),
fhPrimEtaYeta(0x0),          fhPrimEtaYetaYcut(0x0),       fhPrimEtaAccYeta(0x0),
fhPrimEtaPhi(0x0),           fhPrimEtaAccPhi(0x0),
fhPrimEtaOpeningAngle(0x0),  fhPrimEtaOpeningAngleAsym(0x0),fhPrimEtaCosOpeningAngle(0x0),
fhPrimEtaPtCentrality(0),    fhPrimEtaPtEventPlane(0),
fhPrimEtaAccPtCentrality(0), fhPrimEtaAccPtEventPlane(0),
fhPrimPi0PtOrigin(0x0),      fhPrimEtaPtOrigin(0x0),
fhMCOrgMass(),               fhMCOrgAsym(),                fhMCOrgDeltaEta(),            fhMCOrgDeltaPhi(),
fhMCPi0MassPtRec(),          fhMCPi0MassPtTrue(),          fhMCPi0PtTruePtRec(),         
fhMCEtaMassPtRec(),          fhMCEtaMassPtTrue(),          fhMCEtaPtTruePtRec(),
fhMCPi0PtOrigin(0x0),        fhMCEtaPtOrigin(0x0),
fhMCPi0ProdVertex(0),        fhMCEtaProdVertex(0),
fhPrimPi0ProdVertex(0),      fhPrimEtaProdVertex(0),
fhReMCFromConversion(0),     fhReMCFromNotConversion(0),   fhReMCFromMixConversion(0),
fhCosThStarPrimPi0(0),       fhCosThStarPrimEta(0)//,
{
  //Default Ctor
 
  InitParameters();
  
  for(Int_t i = 0; i < 4; i++)
  {
    fhArmPrimEta[i] = 0;
    fhArmPrimPi0[i] = 0;
  }
}

//_____________________
AliAnaPi0::~AliAnaPi0()
{
  // Remove event containers
  
  if(DoOwnMix() && fEventsList)
  {
    for(Int_t ic=0; ic<GetNCentrBin(); ic++)
    {
      for(Int_t iz=0; iz<GetNZvertBin(); iz++)
      {
        for(Int_t irp=0; irp<GetNRPBin(); irp++)
        {
          Int_t bin = GetEventMixBin(ic,iz,irp);
          fEventsList[bin]->Delete() ;
          delete fEventsList[bin] ;
        }
      }
    }
    delete[] fEventsList; 
  }
	
}

//______________________________
void AliAnaPi0::InitParameters()
{
  //Init parameters when first called the analysis
  //Set default parameters
  SetInputAODName("PWG4Particle");
  
  AddToHistogramsName("AnaPi0_");
  
  fCalorimeter  = "PHOS";
  fUseAngleCut = kFALSE;
  fUseAngleEDepCut = kFALSE;
  fAngleCut    = 0.; 
  fAngleMaxCut = TMath::Pi(); 
  
  fMultiCutAna = kFALSE;
  
  fNPtCuts = 1;
  fPtCuts[0] = 0.; fPtCuts[1] = 0.3;   fPtCuts[2] = 0.5;
  for(Int_t i = fNPtCuts; i < 10; i++)fPtCuts[i] = 0.;
  
  fNAsymCuts = 2;
  fAsymCuts[0] = 1.;  fAsymCuts[1] = 0.7; //fAsymCuts[2] = 0.6; //  fAsymCuts[3] = 0.1;    
  for(Int_t i = fNAsymCuts; i < 10; i++)fAsymCuts[i] = 0.;
  
  fNCellNCuts = 1;
  fCellNCuts[0] = 0; fCellNCuts[1] = 1;   fCellNCuts[2] = 2;   
  for(Int_t i = fNCellNCuts; i < 10; i++)fCellNCuts[i]  = 0;
  
  fNPIDBits = 1;
  fPIDBits[0] = 0;   fPIDBits[1] = 2; //  fPIDBits[2] = 4; fPIDBits[3] = 6;// check, no cut,  dispersion, neutral, dispersion&&neutral
  for(Int_t i = fNPIDBits; i < 10; i++)fPIDBits[i] = 0;
  
}


//_______________________________________
TObjString * AliAnaPi0::GetAnalysisCuts()
{  
  //Save parameters used for analysis
  TString parList ; //this will be list of parameters used for this analysis.
  const Int_t buffersize = 255;
  char onePar[buffersize] ;
  snprintf(onePar,buffersize,"--- AliAnaPi0 ---\n") ;
  parList+=onePar ;	
  snprintf(onePar,buffersize,"Number of bins in Centrality:  %d \n",GetNCentrBin()) ;
  parList+=onePar ;
  snprintf(onePar,buffersize,"Number of bins in Z vert. pos: %d \n",GetNZvertBin()) ;
  parList+=onePar ;
  snprintf(onePar,buffersize,"Number of bins in Reac. Plain: %d \n",GetNRPBin()) ;
  parList+=onePar ;
  snprintf(onePar,buffersize,"Depth of event buffer: %d \n",GetNMaxEvMix()) ;
  parList+=onePar ;
  snprintf(onePar,buffersize,"Select pairs with their angle: %d, edep %d, min angle %2.3f, max angle %2.3f,\n",fUseAngleCut, fUseAngleEDepCut,fAngleCut,fAngleMaxCut) ;
  parList+=onePar ;
  snprintf(onePar,buffersize," Asymmetry cuts: n = %d, asymmetry < ",fNAsymCuts) ;
  for(Int_t i = 0; i < fNAsymCuts; i++) snprintf(onePar,buffersize,"%s %2.2f;",onePar,fAsymCuts[i]);
  parList+=onePar ;
  snprintf(onePar,buffersize," PID selection bits: n = %d, PID bit =\n",fNPIDBits) ;
  for(Int_t i = 0; i < fNPIDBits; i++) snprintf(onePar,buffersize,"%s %d;",onePar,fPIDBits[i]);
  parList+=onePar ;
  snprintf(onePar,buffersize,"Cuts: \n") ;
  parList+=onePar ;
  snprintf(onePar,buffersize,"Z vertex position: -%f < z < %f \n",GetZvertexCut(),GetZvertexCut()) ;
  parList+=onePar ;
  snprintf(onePar,buffersize,"Calorimeter: %s \n",fCalorimeter.Data()) ;
  parList+=onePar ;
  snprintf(onePar,buffersize,"Number of modules: %d \n",fNModules) ;
  parList+=onePar ;
  if(fMultiCutAna){
    snprintf(onePar, buffersize," pT cuts: n = %d, pt > ",fNPtCuts) ;
    for(Int_t i = 0; i < fNPtCuts; i++) snprintf(onePar,buffersize,"%s %2.2f;",onePar,fPtCuts[i]);
    parList+=onePar ;
    snprintf(onePar,buffersize, " N cell in cluster cuts: n = %d, nCell > ",fNCellNCuts) ;
    for(Int_t i = 0; i < fNCellNCuts; i++) snprintf(onePar,buffersize,"%s %d;",onePar,fCellNCuts[i]);
    parList+=onePar ;
  }
  
  return new TObjString(parList) ;	
}

//_________________________________________
TList * AliAnaPi0::GetCreateOutputObjects()
{  
  // Create histograms to be saved in output file and 
  // store them in fOutputContainer
  
  // Init the number of modules, set in the class AliCalorimeterUtils
  fNModules = GetCaloUtils()->GetNumberOfSuperModulesUsed();
  if(fCalorimeter=="PHOS" && fNModules > 4) fNModules = 4;
  
  //create event containers
  fEventsList = new TList*[GetNCentrBin()*GetNZvertBin()*GetNRPBin()] ;

  for(Int_t ic=0; ic<GetNCentrBin(); ic++)
  {
    for(Int_t iz=0; iz<GetNZvertBin(); iz++)
    {
      for(Int_t irp=0; irp<GetNRPBin(); irp++)
      {
        Int_t bin = GetEventMixBin(ic,iz,irp);
        fEventsList[bin] = new TList() ;
        fEventsList[bin]->SetOwner(kFALSE);
      }
    }
  }
  
  TList * outputContainer = new TList() ; 
  outputContainer->SetName(GetName()); 
	
  fhReMod                = new TH2F*[fNModules]   ;
  fhMiMod                = new TH2F*[fNModules]   ;
  
  if(fCalorimeter == "PHOS")
  {
    fhReDiffPHOSMod        = new TH2F*[fNModules]   ;  
    fhMiDiffPHOSMod        = new TH2F*[fNModules]   ;
  }
  else
  {
    fhReSameSectorEMCALMod = new TH2F*[fNModules/2] ;
    fhReSameSideEMCALMod   = new TH2F*[fNModules-2] ;  
    fhMiSameSectorEMCALMod = new TH2F*[fNModules/2] ;
    fhMiSameSideEMCALMod   = new TH2F*[fNModules-2] ;
  }
  
  
  fhRe1 = new TH2F*[GetNCentrBin()*fNPIDBits*fNAsymCuts] ;
  fhMi1 = new TH2F*[GetNCentrBin()*fNPIDBits*fNAsymCuts] ;
  if(fFillBadDistHisto)
  {
    fhRe2 = new TH2F*[GetNCentrBin()*fNPIDBits*fNAsymCuts] ;
    fhRe3 = new TH2F*[GetNCentrBin()*fNPIDBits*fNAsymCuts] ;
    fhMi2 = new TH2F*[GetNCentrBin()*fNPIDBits*fNAsymCuts] ;
    fhMi3 = new TH2F*[GetNCentrBin()*fNPIDBits*fNAsymCuts] ;
  }
  if(fMakeInvPtPlots)
  {
    fhReInvPt1 = new TH2F*[GetNCentrBin()*fNPIDBits*fNAsymCuts] ;
    fhMiInvPt1 = new TH2F*[GetNCentrBin()*fNPIDBits*fNAsymCuts] ;
    if(fFillBadDistHisto){
      fhReInvPt2 = new TH2F*[GetNCentrBin()*fNPIDBits*fNAsymCuts] ;
      fhReInvPt3 = new TH2F*[GetNCentrBin()*fNPIDBits*fNAsymCuts] ;
      fhMiInvPt2 = new TH2F*[GetNCentrBin()*fNPIDBits*fNAsymCuts] ;
      fhMiInvPt3 = new TH2F*[GetNCentrBin()*fNPIDBits*fNAsymCuts] ;
    }
  } 
  
  const Int_t buffersize = 255;
  char key[buffersize] ;
  char title[buffersize] ;
  
  Int_t nptbins   = GetHistogramRanges()->GetHistoPtBins();
  Int_t nphibins  = GetHistogramRanges()->GetHistoPhiBins();
  Int_t netabins  = GetHistogramRanges()->GetHistoEtaBins();
  Float_t ptmax   = GetHistogramRanges()->GetHistoPtMax();
  Float_t phimax  = GetHistogramRanges()->GetHistoPhiMax();
  Float_t etamax  = GetHistogramRanges()->GetHistoEtaMax();
  Float_t ptmin   = GetHistogramRanges()->GetHistoPtMin();
  Float_t phimin  = GetHistogramRanges()->GetHistoPhiMin();
  Float_t etamin  = GetHistogramRanges()->GetHistoEtaMin();	
	
  Int_t nmassbins = GetHistogramRanges()->GetHistoMassBins();
  Int_t nasymbins = GetHistogramRanges()->GetHistoAsymmetryBins();
  Float_t massmax = GetHistogramRanges()->GetHistoMassMax();
  Float_t asymmax = GetHistogramRanges()->GetHistoAsymmetryMax();
  Float_t massmin = GetHistogramRanges()->GetHistoMassMin();
  Float_t asymmin = GetHistogramRanges()->GetHistoAsymmetryMin();
  Int_t ntrmbins  = GetHistogramRanges()->GetHistoTrackMultiplicityBins();
  Int_t ntrmmax   = GetHistogramRanges()->GetHistoTrackMultiplicityMax();
  Int_t ntrmmin   = GetHistogramRanges()->GetHistoTrackMultiplicityMin(); 
    
  if(fCheckConversion)
  {
    fhReConv = new TH2F("hReConv","Real Pair with one recombined conversion ",nptbins,ptmin,ptmax,nmassbins,massmin,massmax) ;
    fhReConv->SetXTitle("#it{p}_{T} (GeV/#it{c})");
    fhReConv->SetYTitle("#it{M}_{#gamma,#gamma} (GeV/#it{c}^{2})");
    outputContainer->Add(fhReConv) ;
    
    fhReConv2 = new TH2F("hReConv2","Real Pair with 2 recombined conversion ",nptbins,ptmin,ptmax,nmassbins,massmin,massmax) ;
    fhReConv2->SetXTitle("#it{p}_{T} (GeV/#it{c})");
    fhReConv2->SetYTitle("#it{M}_{#gamma,#gamma} (GeV/#it{c}^{2})");
    outputContainer->Add(fhReConv2) ;
    
    if(DoOwnMix())
    {
      fhMiConv = new TH2F("hMiConv","Mixed Pair with one recombined conversion ",nptbins,ptmin,ptmax,nmassbins,massmin,massmax) ;
      fhMiConv->SetXTitle("#it{p}_{T} (GeV/#it{c})");
      fhMiConv->SetYTitle("#it{M}_{#gamma,#gamma} (GeV/#it{c}^{2})");
      outputContainer->Add(fhMiConv) ;
      
      fhMiConv2 = new TH2F("hMiConv2","Mixed Pair with 2 recombined conversion ",nptbins,ptmin,ptmax,nmassbins,massmin,massmax) ;
      fhMiConv2->SetXTitle("#it{p}_{T} (GeV/#it{c})");
      fhMiConv2->SetYTitle("#it{M}_{#gamma,#gamma} (GeV/#it{c}^{2})");
      outputContainer->Add(fhMiConv2) ;
    }
  }
  
  for(Int_t ic=0; ic<GetNCentrBin(); ic++)
  {
    for(Int_t ipid=0; ipid<fNPIDBits; ipid++)
    {
      for(Int_t iasym=0; iasym<fNAsymCuts; iasym++)
      {
        Int_t index = ((ic*fNPIDBits)+ipid)*fNAsymCuts + iasym;
        //printf("cen %d, pid %d, asy %d, Index %d\n",ic,ipid,iasym,index);
        //Distance to bad module 1
        snprintf(key, buffersize,"hRe_cen%d_pidbit%d_asy%d_dist1",ic,ipid,iasym) ;
        snprintf(title, buffersize,"Real #it{M}_{#gamma#gamma} distr. for centrality=%d, PID bit=%d and asymmetry %1.2f, dist bad 1",
                 ic,fPIDBits[ipid], fAsymCuts[iasym]) ;
        fhRe1[index] = new TH2F(key,title,nptbins,ptmin,ptmax,nmassbins,massmin,massmax) ;
        fhRe1[index]->SetXTitle("#it{p}_{T} (GeV/#it{c})");
        fhRe1[index]->SetYTitle("#it{M}_{#gamma,#gamma} (GeV/#it{c}^{2})");
        //printf("name: %s\n ",fhRe1[index]->GetName());
        outputContainer->Add(fhRe1[index]) ;
        
        if(fFillBadDistHisto)
        {
          //Distance to bad module 2
          snprintf(key, buffersize,"hRe_cen%d_pidbit%d_asy%d_dist2",ic,ipid,iasym) ;
          snprintf(title, buffersize,"Real #it{M}_{#gamma#gamma} distr. for centrality=%d, PID bit=%d and asymmetry %1.2f, dist bad 2",
                   ic,fPIDBits[ipid], fAsymCuts[iasym]) ;
          fhRe2[index] = new TH2F(key,title,nptbins,ptmin,ptmax,nmassbins,massmin,massmax) ;
          fhRe2[index]->SetXTitle("#it{p}_{T} (GeV/#it{c})");
          fhRe2[index]->SetYTitle("#it{M}_{#gamma,#gamma} (GeV/#it{c}^{2})");
          outputContainer->Add(fhRe2[index]) ;
          
          //Distance to bad module 3
          snprintf(key, buffersize,"hRe_cen%d_pidbit%d_asy%d_dist3",ic,ipid,iasym) ;
          snprintf(title, buffersize,"Real #it{M}_{#gamma#gamma} distr. for centrality=%d, PID bit=%d and asymmetry %1.2f, dist bad 3",
                   ic,fPIDBits[ipid], fAsymCuts[iasym]) ;
          fhRe3[index] = new TH2F(key,title,nptbins,ptmin,ptmax,nmassbins,massmin,massmax) ;
          fhRe3[index]->SetXTitle("#it{p}_{T} (GeV/#it{c})");
          fhRe3[index]->SetYTitle("#it{M}_{#gamma,#gamma} (GeV/#it{c}^{2})");
          outputContainer->Add(fhRe3[index]) ;
        }
        
        //Inverse pT 
        if(fMakeInvPtPlots)
        {
          //Distance to bad module 1
          snprintf(key, buffersize,"hReInvPt_cen%d_pidbit%d_asy%d_dist1",ic,ipid,iasym) ;
          snprintf(title, buffersize,"Real #it{M}_{#gamma#gamma} distr. for centrality=%d, PID bit=%d and asymmetry %1.2f, dist bad 1",
                   ic,fPIDBits[ipid], fAsymCuts[iasym]) ;
          fhReInvPt1[index] = new TH2F(key,title,nptbins,ptmin,ptmax,nmassbins,massmin,massmax) ;
          fhReInvPt1[index]->SetXTitle("#it{p}_{T} (GeV/#it{c})");
          fhReInvPt1[index]->SetYTitle("#it{M}_{#gamma,#gamma} (GeV/#it{c}^{2})");
          outputContainer->Add(fhReInvPt1[index]) ;
          
          if(fFillBadDistHisto){
            //Distance to bad module 2
            snprintf(key, buffersize,"hReInvPt_cen%d_pidbit%d_asy%d_dist2",ic,ipid,iasym) ;
            snprintf(title, buffersize,"Real #it{M}_{#gamma#gamma} distr. for centrality=%d, PID bit=%d and asymmetry %1.2f, dist bad 2",
                     ic,fPIDBits[ipid], fAsymCuts[iasym]) ;
            fhReInvPt2[index] = new TH2F(key,title,nptbins,ptmin,ptmax,nmassbins,massmin,massmax) ;
            fhReInvPt2[index]->SetXTitle("#it{p}_{T} (GeV/#it{c})");
            fhReInvPt2[index]->SetYTitle("#it{M}_{#gamma,#gamma} (GeV/#it{c}^{2})");
            outputContainer->Add(fhReInvPt2[index]) ;
            
            //Distance to bad module 3
            snprintf(key, buffersize,"hReInvPt_cen%d_pidbit%d_asy%d_dist3",ic,ipid,iasym) ;
            snprintf(title, buffersize,"Real #it{M}_{#gamma#gamma} distr. for centrality=%d, PID bit=%d and asymmetry %1.2f, dist bad 3",
                     ic,fPIDBits[ipid], fAsymCuts[iasym]) ;
            fhReInvPt3[index] = new TH2F(key,title,nptbins,ptmin,ptmax,nmassbins,massmin,massmax) ;
            fhReInvPt3[index]->SetXTitle("#it{p}_{T} (GeV/#it{c})");
            fhReInvPt3[index]->SetYTitle("#it{M}_{#gamma,#gamma} (GeV/#it{c}^{2})");
            outputContainer->Add(fhReInvPt3[index]) ;
          }
        }
        
        if(DoOwnMix())
        {
          //Distance to bad module 1
          snprintf(key, buffersize,"hMi_cen%d_pidbit%d_asy%d_dist1",ic,ipid,iasym) ;
          snprintf(title, buffersize,"Mixed #it{M}_{#gamma#gamma} distr. for centrality=%d, PID bit=%d and asymmetry %1.2f, dist bad 1",
                   ic,fPIDBits[ipid], fAsymCuts[iasym]) ;
          fhMi1[index] = new TH2F(key,title,nptbins,ptmin,ptmax,nmassbins,massmin,massmax) ;
          fhMi1[index]->SetXTitle("#it{p}_{T} (GeV/#it{c})");
          fhMi1[index]->SetYTitle("#it{M}_{#gamma,#gamma} (GeV/#it{c}^{2})");
          outputContainer->Add(fhMi1[index]) ;
          if(fFillBadDistHisto){
            //Distance to bad module 2
            snprintf(key, buffersize,"hMi_cen%d_pidbit%d_asy%d_dist2",ic,ipid,iasym) ;
            snprintf(title, buffersize,"Mixed #it{M}_{#gamma#gamma} distr. for centrality=%d, PID bit=%d and asymmetry %1.2f, dist bad 2",
                     ic,fPIDBits[ipid], fAsymCuts[iasym]) ;
            fhMi2[index] = new TH2F(key,title,nptbins,ptmin,ptmax,nmassbins,massmin,massmax) ;
            fhMi2[index]->SetXTitle("#it{p}_{T} (GeV/#it{c})");
            fhMi2[index]->SetYTitle("#it{M}_{#gamma,#gamma} (GeV/#it{c}^{2})");
            outputContainer->Add(fhMi2[index]) ;
            
            //Distance to bad module 3
            snprintf(key, buffersize,"hMi_cen%d_pidbit%d_asy%d_dist3",ic,ipid,iasym) ;
            snprintf(title, buffersize,"Mixed #it{M}_{#gamma#gamma} distr. for centrality=%d, PID bit=%d and asymmetry %1.2f, dist bad 3",
                     ic,fPIDBits[ipid], fAsymCuts[iasym]) ;
            fhMi3[index] = new TH2F(key,title,nptbins,ptmin,ptmax,nmassbins,massmin,massmax) ;
            fhMi3[index]->SetXTitle("#it{p}_{T} (GeV/#it{c})");
            fhMi3[index]->SetYTitle("#it{M}_{#gamma,#gamma} (GeV/#it{c}^{2})");
            outputContainer->Add(fhMi3[index]) ;
          }
          
          //Inverse pT
          if(fMakeInvPtPlots)
          {
            //Distance to bad module 1
            snprintf(key, buffersize,"hMiInvPt_cen%d_pidbit%d_asy%d_dist1",ic,ipid,iasym) ;
            snprintf(title, buffersize,"Mixed #it{M}_{#gamma#gamma} distr. for centrality=%d, PID bit=%d and asymmetry %1.2f, dist bad 1",
                     ic,fPIDBits[ipid], fAsymCuts[iasym]) ;
            fhMiInvPt1[index] = new TH2F(key,title,nptbins,ptmin,ptmax,nmassbins,massmin,massmax) ;
            fhMiInvPt1[index]->SetXTitle("#it{p}_{T} (GeV/#it{c})");
            fhMiInvPt1[index]->SetYTitle("#it{M}_{#gamma,#gamma} (GeV/#it{c}^{2})");
            outputContainer->Add(fhMiInvPt1[index]) ;
            if(fFillBadDistHisto){
              //Distance to bad module 2
              snprintf(key, buffersize,"hMiInvPt_cen%d_pidbit%d_asy%d_dist2",ic,ipid,iasym) ;
              snprintf(title, buffersize,"Mixed #it{M}_{#gamma#gamma} distr. for centrality=%d, PID bit=%d and asymmetry %1.2f, dist bad 2",
                       ic,fPIDBits[ipid], fAsymCuts[iasym]) ;
              fhMiInvPt2[index] = new TH2F(key,title,nptbins,ptmin,ptmax,nmassbins,massmin,massmax) ;
              fhMiInvPt2[index]->SetXTitle("#it{p}_{T} (GeV/#it{c})");
              fhMiInvPt2[index]->SetYTitle("#it{M}_{#gamma,#gamma} (GeV/#it{c}^{2})");
              outputContainer->Add(fhMiInvPt2[index]) ;
              
              //Distance to bad module 3
              snprintf(key, buffersize,"hMiInvPt_cen%d_pidbit%d_asy%d_dist3",ic,ipid,iasym) ;
              snprintf(title, buffersize,"Mixed #it{M}_{#gamma#gamma} distr. for centrality=%d, PID bit=%d and asymmetry %1.2f,dist bad 3",
                       ic,fPIDBits[ipid], fAsymCuts[iasym]) ;
              fhMiInvPt3[index] = new TH2F(key,title,nptbins,ptmin,ptmax,nmassbins,massmin,massmax) ;
              fhMiInvPt3[index]->SetXTitle("#it{p}_{T} (GeV/#it{c})");
              fhMiInvPt3[index]->SetYTitle("#it{M}_{#gamma,#gamma} (GeV/#it{c}^{2})");
              outputContainer->Add(fhMiInvPt3[index]) ;
            }
          }
        } 
      }
    }
  }
  
  if(fFillAsymmetryHisto)
  {
    fhRePtAsym = new TH2F("hRePtAsym","#it{Asymmetry} vs #it{p}_{T} , for pairs",nptbins,ptmin,ptmax,nasymbins,asymmin,asymmax) ;
    fhRePtAsym->SetXTitle("#it{p}_{T} (GeV/#it{c})");
    fhRePtAsym->SetYTitle("#it{Asymmetry}");
    outputContainer->Add(fhRePtAsym);
    
    fhRePtAsymPi0 = new TH2F("hRePtAsymPi0","#it{Asymmetry} vs #it{p}_{T} , for pairs close to #pi^{0} mass",nptbins,ptmin,ptmax,nasymbins,asymmin,asymmax) ;
    fhRePtAsymPi0->SetXTitle("#it{p}_{T} (GeV/#it{c})");
    fhRePtAsymPi0->SetYTitle("Asymmetry");
    outputContainer->Add(fhRePtAsymPi0);
    
    fhRePtAsymEta = new TH2F("hRePtAsymEta","#it{Asymmetry} vs #it{p}_{T} , for pairs close to #eta mass",nptbins,ptmin,ptmax,nasymbins,asymmin,asymmax) ;
    fhRePtAsymEta->SetXTitle("#it{p}_{T} (GeV/#it{c})");
    fhRePtAsymEta->SetYTitle("#it{Asymmetry}");
    outputContainer->Add(fhRePtAsymEta);
  }
  
  if(fMultiCutAna)
  {
    fhRePIDBits         = new TH2F*[fNPIDBits];
    for(Int_t ipid=0; ipid<fNPIDBits; ipid++){
      snprintf(key,   buffersize,"hRe_pidbit%d",ipid) ;
      snprintf(title, buffersize,"Real #it{M}_{#gamma#gamma} distr. for PIDBit=%d",fPIDBits[ipid]) ;
      fhRePIDBits[ipid] = new TH2F(key,title,nptbins,ptmin,ptmax,nmassbins,massmin,massmax) ;
      fhRePIDBits[ipid]->SetXTitle("#it{p}_{T} (GeV/#it{c})");
      fhRePIDBits[ipid]->SetYTitle("#it{M}_{#gamma,#gamma} (GeV/#it{c}^{2})");
      outputContainer->Add(fhRePIDBits[ipid]) ;
    }// pid bit loop
    
    fhRePtNCellAsymCuts    = new TH2F*[fNPtCuts*fNAsymCuts*fNCellNCuts];
    fhMiPtNCellAsymCuts    = new TH2F*[fNPtCuts*fNAsymCuts*fNCellNCuts];
    
    if(fFillSMCombinations)
      for(Int_t iSM = 0; iSM < fNModules; iSM++) fhRePtNCellAsymCutsSM[iSM] = new TH2F*[fNPtCuts*fNAsymCuts*fNCellNCuts];
      
    
    for(Int_t ipt=0; ipt<fNPtCuts; ipt++)
    {
      for(Int_t icell=0; icell<fNCellNCuts; icell++)
      {
        for(Int_t iasym=0; iasym<fNAsymCuts; iasym++)
        {
          snprintf(key,   buffersize,"hRe_pt%d_cell%d_asym%d",ipt,icell,iasym) ;
          snprintf(title, buffersize,"Real #it{M}_{#gamma#gamma} distr. for pt >%2.2f, ncell>%d and asym >%1.2f ",fPtCuts[ipt],fCellNCuts[icell], fAsymCuts[iasym]) ;
          Int_t index = ((ipt*fNCellNCuts)+icell)*fNAsymCuts + iasym;
          //printf("ipt %d, icell %d, iassym %d, index %d\n",ipt, icell, iasym, index);
          fhRePtNCellAsymCuts[index] = new TH2F(key,title,nptbins,ptmin,ptmax,nmassbins,massmin,massmax) ;
          fhRePtNCellAsymCuts[index]->SetXTitle("#it{p}_{T} (GeV/#it{c})");
          fhRePtNCellAsymCuts[index]->SetYTitle("#it{M}_{#gamma,#gamma} (GeV/#it{c}^{2})");
          outputContainer->Add(fhRePtNCellAsymCuts[index]) ;
          
          snprintf(key,   buffersize,"hMi_pt%d_cell%d_asym%d",ipt,icell,iasym) ;
          snprintf(title, buffersize,"Mixed #it{M}_{#gamma#gamma} distr. for pt >%2.2f, ncell>%d and asym >%1.2f",fPtCuts[ipt],fCellNCuts[icell], fAsymCuts[iasym]) ;
          fhMiPtNCellAsymCuts[index] = new TH2F(key,title,nptbins,ptmin,ptmax,nmassbins,massmin,massmax) ;
          fhMiPtNCellAsymCuts[index]->SetXTitle("#it{p}_{T} (GeV/#it{c})");
          fhMiPtNCellAsymCuts[index]->SetYTitle("#it{M}_{#gamma,#gamma} (GeV/#it{c}^{2})");
          outputContainer->Add(fhMiPtNCellAsymCuts[index]) ;          
          
          if(fFillSMCombinations)
          {
            for(Int_t iSM = 0; iSM < fNModules; iSM++)
            {
              snprintf(key,   buffersize,"hRe_pt%d_cell%d_asym%d_SM%d",ipt,icell,iasym,iSM) ;
              snprintf(title, buffersize,"Real #it{M}_{#gamma#gamma} distr. for pt >%2.2f, ncell>%d and asym >%1.2f, SM %d ",
                       fPtCuts[ipt],fCellNCuts[icell], fAsymCuts[iasym],iSM) ;
              fhRePtNCellAsymCutsSM[iSM][index] = new TH2F(key,title,nptbins,ptmin,ptmax,nmassbins,massmin,massmax) ;
              fhRePtNCellAsymCutsSM[iSM][index]->SetXTitle("#it{p}_{T} (GeV/#it{c})");
              fhRePtNCellAsymCutsSM[iSM][index]->SetYTitle("#it{M}_{#gamma,#gamma} (GeV/#it{c}^{2})");
              outputContainer->Add(fhRePtNCellAsymCutsSM[iSM][index]) ;
            }
            
          }
        }
      }
    }
    
    if(ntrmbins!=0)
    {
      fhRePtMult = new TH3F*[fNAsymCuts] ;
      for(Int_t iasym = 0; iasym<fNAsymCuts; iasym++)
      {
        fhRePtMult[iasym] = new TH3F(Form("hRePtMult_asym%d",iasym),Form("(#it{p}_{T},C,M)_{#gamma#gamma}, A<%1.2f",fAsymCuts[iasym]),
                                     nptbins,ptmin,ptmax,ntrmbins,ntrmmin,ntrmmax,nmassbins,massmin,massmax);
        fhRePtMult[iasym]->SetXTitle("#it{p}_{T} (GeV/#it{c})");
        fhRePtMult[iasym]->SetYTitle("Track multiplicity");
        fhRePtMult[iasym]->SetZTitle("#it{M}_{#gamma,#gamma} (GeV/#it{c}^{2})");
        outputContainer->Add(fhRePtMult[iasym]) ;
      }
    }
  }// multi cuts analysis
  
  if(fFillSSCombinations)
  {
    
    fhReSS[0] = new TH2F("hRe_SS_Tight"," 0.01 < #lambda_{0}^{2} < 0.4",
                         nptbins,ptmin,ptmax,nmassbins,massmin,massmax);
    fhReSS[0]->SetXTitle("#it{p}_{T} (GeV/#it{c})");
    fhReSS[0]->SetYTitle("#it{M}_{#gamma,#gamma} (GeV/#it{c}^{2})");
    outputContainer->Add(fhReSS[0]) ;
    
    
    fhReSS[1] = new TH2F("hRe_SS_Loose"," #lambda_{0}^{2} > 0.4",
                         nptbins,ptmin,ptmax,nmassbins,massmin,massmax);
    fhReSS[1]->SetXTitle("#it{p}_{T} (GeV/#it{c})");
    fhReSS[1]->SetYTitle("#it{M}_{#gamma,#gamma} (GeV/#it{c}^{2})");
    outputContainer->Add(fhReSS[1]) ;
    
    
    fhReSS[2] = new TH2F("hRe_SS_Both"," cluster_{1} #lambda_{0}^{2} > 0.4; cluster_{2} 0.01 < #lambda_{0}^{2} < 0.4",
                         nptbins,ptmin,ptmax,nmassbins,massmin,massmax);
    fhReSS[2]->SetXTitle("#it{p}_{T} (GeV/#it{c})");
    fhReSS[2]->SetYTitle("#it{M}_{#gamma,#gamma} (GeV/#it{c}^{2})");
    outputContainer->Add(fhReSS[2]) ;
  }
  
  if(DoOwnMix())
  {
    fhEventBin=new TH1I("hEventBin","Number of real pairs per bin(cen,vz,rp)",
                        GetNCentrBin()*GetNZvertBin()*GetNRPBin()+1,0,
                        GetNCentrBin()*GetNZvertBin()*GetNRPBin()+1) ;
    fhEventBin->SetXTitle("bin");
    outputContainer->Add(fhEventBin) ;
    
    
    fhEventMixBin=new TH1I("hEventMixBin","Number of mixed pairs per bin(cen,vz,rp)",
                           GetNCentrBin()*GetNZvertBin()*GetNRPBin()+1,0,
                           GetNCentrBin()*GetNZvertBin()*GetNRPBin()+1) ;
    fhEventMixBin->SetXTitle("bin");
    outputContainer->Add(fhEventMixBin) ;
	}
  
  if(GetNCentrBin()>1)
  {
    fhCentrality=new TH1F("hCentralityBin","Number of events in centrality bin",GetNCentrBin(),0.,1.*GetNCentrBin()) ;
    fhCentrality->SetXTitle("Centrality bin");
    outputContainer->Add(fhCentrality) ;
    
    fhCentralityNoPair=new TH1F("hCentralityBinNoPair","Number of events in centrality bin, with no cluster pairs",GetNCentrBin(),0.,1.*GetNCentrBin()) ;
    fhCentralityNoPair->SetXTitle("Centrality bin");
    outputContainer->Add(fhCentralityNoPair) ;
  }
  
  if(GetNRPBin() > 1 && GetNCentrBin()>1 )
  {
    fhEventPlaneResolution=new TH2F("hEventPlaneResolution","Event plane resolution",GetNCentrBin(),0,GetNCentrBin(),100,0.,TMath::TwoPi()) ;
    fhEventPlaneResolution->SetYTitle("Resolution");
    fhEventPlaneResolution->SetXTitle("Centrality Bin");
    outputContainer->Add(fhEventPlaneResolution) ;
  }
  
  if(fFillAngleHisto)
  {
    fhRealOpeningAngle  = new TH2F
    ("hRealOpeningAngle","Angle between all #gamma pair vs E_{#pi^{0}}",nptbins,ptmin,ptmax,300,0,TMath::Pi()); 
    fhRealOpeningAngle->SetYTitle("#theta(rad)");
    fhRealOpeningAngle->SetXTitle("E_{ #pi^{0}} (GeV)");
    outputContainer->Add(fhRealOpeningAngle) ;
    
    fhRealCosOpeningAngle  = new TH2F
    ("hRealCosOpeningAngle","Cosinus of angle between all #gamma pair vs E_{#pi^{0}}",nptbins,ptmin,ptmax,100,0,1); 
    fhRealCosOpeningAngle->SetYTitle("cos (#theta) ");
    fhRealCosOpeningAngle->SetXTitle("E_{ #pi^{0}} (GeV)");
    outputContainer->Add(fhRealCosOpeningAngle) ;
    
    if(DoOwnMix())
    {
      fhMixedOpeningAngle  = new TH2F
      ("hMixedOpeningAngle","Angle between all #gamma pair vs E_{#pi^{0}}, Mixed pairs",nptbins,ptmin,ptmax,300,0,TMath::Pi()); 
      fhMixedOpeningAngle->SetYTitle("#theta(rad)");
      fhMixedOpeningAngle->SetXTitle("E_{ #pi^{0}} (GeV)");
      outputContainer->Add(fhMixedOpeningAngle) ;
      
      fhMixedCosOpeningAngle  = new TH2F
      ("hMixedCosOpeningAngle","Cosinus of angle between all #gamma pair vs E_{#pi^{0}}, Mixed pairs",nptbins,ptmin,ptmax,100,0,1); 
      fhMixedCosOpeningAngle->SetYTitle("cos (#theta) ");
      fhMixedCosOpeningAngle->SetXTitle("E_{ #pi^{0}} (GeV)");
      outputContainer->Add(fhMixedCosOpeningAngle) ;
      
    }
  } 
  
  //Histograms filled only if MC data is requested 	
  if(IsDataMC())
  {
    fhReMCFromConversion = new TH2F("hReMCFromConversion","Invariant mass of 2 clusters originated in conversions",
                         nptbins,ptmin,ptmax,nmassbins,massmin,massmax);
    fhReMCFromConversion->SetXTitle("#it{p}_{T} (GeV/#it{c})");
    fhReMCFromConversion->SetYTitle("#it{M}_{#gamma,#gamma} (GeV/#it{c}^{2})");
    outputContainer->Add(fhReMCFromConversion) ;
    
    fhReMCFromNotConversion = new TH2F("hReMCNotFromConversion","Invariant mass of 2 clusters not originated in conversions",
                                    nptbins,ptmin,ptmax,nmassbins,massmin,massmax);
    fhReMCFromNotConversion->SetXTitle("#it{p}_{T} (GeV/#it{c})");
    fhReMCFromNotConversion->SetYTitle("#it{M}_{#gamma,#gamma} (GeV/#it{c}^{2})");
    outputContainer->Add(fhReMCFromNotConversion) ;
    
    fhReMCFromMixConversion = new TH2F("hReMCFromMixConversion","Invariant mass of 2 clusters one from conversion and the other not",
                                    nptbins,ptmin,ptmax,nmassbins,massmin,massmax);
    fhReMCFromMixConversion->SetXTitle("#it{p}_{T} (GeV/#it{c})");
    fhReMCFromMixConversion->SetYTitle("#it{M}_{#gamma,#gamma} (GeV/#it{c}^{2})");
    outputContainer->Add(fhReMCFromMixConversion) ;
    
    //Pi0

    fhPrimPi0E     = new TH1F("hPrimPi0E","Primary #pi^{0} E, |#it{Y}|<1",nptbins,ptmin,ptmax) ;
    fhPrimPi0AccE  = new TH1F("hPrimPi0AccE","Primary #pi^{0} #it{E} with both photons in acceptance",nptbins,ptmin,ptmax) ;
    fhPrimPi0E   ->SetXTitle("#it{E} (GeV)");
    fhPrimPi0AccE->SetXTitle("#it{E} (GeV)");
    outputContainer->Add(fhPrimPi0E) ;
    outputContainer->Add(fhPrimPi0AccE) ;
    
    fhPrimPi0Pt     = new TH1F("hPrimPi0Pt","Primary #pi^{0} #it{p}_{T} , |#it{Y}|<1",nptbins,ptmin,ptmax) ;
    fhPrimPi0AccPt  = new TH1F("hPrimPi0AccPt","Primary #pi^{0} #it{p}_{T} with both photons in acceptance",nptbins,ptmin,ptmax) ;
    fhPrimPi0Pt   ->SetXTitle("#it{p}_{T} (GeV/#it{c})");
    fhPrimPi0AccPt->SetXTitle("#it{p}_{T} (GeV/#it{c})");
    outputContainer->Add(fhPrimPi0Pt) ;
    outputContainer->Add(fhPrimPi0AccPt) ;
    
    Int_t netabinsopen =  TMath::Nint(netabins*4/(etamax-etamin));
    fhPrimPi0Y      = new TH2F("hPrimPi0Rapidity","Rapidity of primary #pi^{0}",nptbins,ptmin,ptmax,netabinsopen,-2, 2) ;
    fhPrimPi0Y   ->SetYTitle("#it{Rapidity}");
    fhPrimPi0Y   ->SetXTitle("#it{p}_{T} (GeV/#it{c})");
    outputContainer->Add(fhPrimPi0Y) ;

    fhPrimPi0Yeta      = new TH2F("hPrimPi0PseudoRapidity","PseudoRapidity of primary #pi^{0}",nptbins,ptmin,ptmax,netabinsopen,-2, 2) ;
    fhPrimPi0Yeta   ->SetYTitle("#eta");
    fhPrimPi0Yeta   ->SetXTitle("#it{p}_{T} (GeV/#it{c})");
    outputContainer->Add(fhPrimPi0Yeta) ;

    fhPrimPi0YetaYcut      = new TH2F("hPrimPi0PseudoRapidityYcut","PseudoRapidity of primary #pi^{0}, |#it{Y}|<1",nptbins,ptmin,ptmax,netabinsopen,-2, 2) ;
    fhPrimPi0YetaYcut   ->SetYTitle("#eta");
    fhPrimPi0YetaYcut   ->SetXTitle("#it{p}_{T} (GeV/#it{c})");
    outputContainer->Add(fhPrimPi0YetaYcut) ;
    
    fhPrimPi0AccY   = new TH2F("hPrimPi0AccRapidity","Rapidity of primary #pi^{0} with accepted daughters",nptbins,ptmin,ptmax,netabins,etamin,etamax) ;
    fhPrimPi0AccY->SetYTitle("Rapidity");
    fhPrimPi0AccY->SetXTitle("#it{p}_{T} (GeV/#it{c})");
    outputContainer->Add(fhPrimPi0AccY) ;
    
    fhPrimPi0AccYeta      = new TH2F("hPrimPi0AccPseudoRapidity","PseudoRapidity of primary #pi^{0} with accepted daughters",nptbins,ptmin,ptmax,netabins,etamin,etamax) ;
    fhPrimPi0AccYeta   ->SetYTitle("#eta");
    fhPrimPi0AccYeta   ->SetXTitle("#it{p}_{T} (GeV/#it{c})");
    outputContainer->Add(fhPrimPi0AccYeta) ;
    
    Int_t nphibinsopen = TMath::Nint(nphibins*TMath::TwoPi()/(phimax-phimin));
    fhPrimPi0Phi    = new TH2F("hPrimPi0Phi","#phi of primary #pi^{0}, |#it{Y}|<1",nptbins,ptmin,ptmax,nphibinsopen,0,360) ;
    fhPrimPi0Phi->SetYTitle("#phi (deg)");
    fhPrimPi0Phi->SetXTitle("#it{p}_{T} (GeV/#it{c})");
    outputContainer->Add(fhPrimPi0Phi) ;
    
    fhPrimPi0AccPhi = new TH2F("hPrimPi0AccPhi","#phi of primary #pi^{0} with accepted daughters",nptbins,ptmin,ptmax,
                               nphibins,phimin*TMath::RadToDeg(),phimax*TMath::RadToDeg()) ; 
    fhPrimPi0AccPhi->SetYTitle("#phi (deg)");
    fhPrimPi0AccPhi->SetXTitle("#it{p}_{T} (GeV/#it{c})");
    outputContainer->Add(fhPrimPi0AccPhi) ;
    
    fhPrimPi0PtCentrality     = new TH2F("hPrimPi0PtCentrality","Primary #pi^{0} #it{p}_{T} vs reco centrality, |#it{Y}|<1",
                                         nptbins,ptmin,ptmax, 100, 0, 100) ;
    fhPrimPi0AccPtCentrality  = new TH2F("hPrimPi0AccPtCentrality","Primary #pi^{0} with both photons in acceptance #it{p}_{T} vs reco centrality",
                                         nptbins,ptmin,ptmax, 100, 0, 100) ;
    fhPrimPi0PtCentrality   ->SetXTitle("#it{p}_{T} (GeV/#it{c})");
    fhPrimPi0AccPtCentrality->SetXTitle("#it{p}_{T} (GeV/#it{c})");
    fhPrimPi0PtCentrality   ->SetYTitle("Centrality");
    fhPrimPi0AccPtCentrality->SetYTitle("Centrality");
    outputContainer->Add(fhPrimPi0PtCentrality) ;
    outputContainer->Add(fhPrimPi0AccPtCentrality) ;
    
    fhPrimPi0PtEventPlane     = new TH2F("hPrimPi0PtEventPlane","Primary #pi^{0} #it{p}_{T} vs reco event plane angle, |#it{Y}|<1",
                                         nptbins,ptmin,ptmax, 100, 0, TMath::Pi()) ;
    fhPrimPi0AccPtEventPlane  = new TH2F("hPrimPi0AccPtEventPlane","Primary #pi^{0} with both photons in acceptance #it{p}_{T} vs reco event plane angle",
                                         nptbins,ptmin,ptmax, 100, 0, TMath::Pi()) ;
    fhPrimPi0PtEventPlane   ->SetXTitle("#it{p}_{T} (GeV/#it{c})");
    fhPrimPi0AccPtEventPlane->SetXTitle("#it{p}_{T} (GeV/#it{c})");
    fhPrimPi0PtEventPlane   ->SetYTitle("Event Plane Angle (rad)");
    fhPrimPi0AccPtEventPlane->SetYTitle("Event Plane Angle (rad)");
    outputContainer->Add(fhPrimPi0PtEventPlane) ;
    outputContainer->Add(fhPrimPi0AccPtEventPlane) ;
    
    //Eta

    fhPrimEtaE     = new TH1F("hPrimEtaE","Primary eta E",nptbins,ptmin,ptmax) ;
    fhPrimEtaAccE  = new TH1F("hPrimEtaAccE","Primary #eta #it{E} with both photons in acceptance",nptbins,ptmin,ptmax) ;
    fhPrimEtaE   ->SetXTitle("#it{E} (GeV)");
    fhPrimEtaAccE->SetXTitle("#it{E} (GeV)");
    outputContainer->Add(fhPrimEtaE) ;
    outputContainer->Add(fhPrimEtaAccE) ;
    
    fhPrimEtaPt     = new TH1F("hPrimEtaPt","Primary #eta #it{p}_{T}",nptbins,ptmin,ptmax) ;
    fhPrimEtaAccPt  = new TH1F("hPrimEtaAccPt","Primary eta #it{p}_{T} with both photons in acceptance",nptbins,ptmin,ptmax) ;
    fhPrimEtaPt   ->SetXTitle("#it{p}_{T} (GeV/#it{c})");
    fhPrimEtaAccPt->SetXTitle("#it{p}_{T} (GeV/#it{c})");
    outputContainer->Add(fhPrimEtaPt) ;
    outputContainer->Add(fhPrimEtaAccPt) ;
    
    fhPrimEtaY      = new TH2F("hPrimEtaRapidity","Rapidity of primary #eta",nptbins,ptmin,ptmax,netabinsopen,-2, 2) ;
    fhPrimEtaY->SetYTitle("#it{Rapidity}");
    fhPrimEtaY->SetXTitle("#it{p}_{T} (GeV/#it{c})");
    outputContainer->Add(fhPrimEtaY) ;

    fhPrimEtaYeta      = new TH2F("hPrimEtaPseudoRapidityEta","PseudoRapidity of primary #eta",nptbins,ptmin,ptmax,netabinsopen,-2, 2) ;
    fhPrimEtaYeta->SetYTitle("#it{Rapidity}");
    fhPrimEtaYeta->SetXTitle("#it{p}_{T} (GeV/#it{c})");
    outputContainer->Add(fhPrimEtaYeta) ;

    fhPrimEtaYetaYcut      = new TH2F("hPrimEtaPseudoRapidityEtaYcut","PseudoRapidity of primary #eta, |#it{Y}|<1",nptbins,ptmin,ptmax,netabinsopen,-2, 2) ;
    fhPrimEtaYetaYcut->SetYTitle("#it{Pseudorapidity}");
    fhPrimEtaYetaYcut->SetXTitle("#it{p}_{T} (GeV/#it{c})");
    outputContainer->Add(fhPrimEtaYetaYcut) ;
    
    fhPrimEtaAccY   = new TH2F("hPrimEtaAccRapidity","Rapidity of primary #eta",nptbins,ptmin,ptmax, netabins,etamin,etamax) ;
    fhPrimEtaAccY->SetYTitle("#it{Rapidity}");
    fhPrimEtaAccY->SetXTitle("#it{p}_{T} (GeV/#it{c})");
    outputContainer->Add(fhPrimEtaAccY) ;
 
    fhPrimEtaAccYeta  = new TH2F("hPrimEtaAccPseudoRapidity","PseudoRapidity of primary #eta",nptbins,ptmin,ptmax, netabins,etamin,etamax) ;
    fhPrimEtaAccYeta->SetYTitle("#it{Pseudorapidity}");
    fhPrimEtaAccYeta->SetXTitle("#it{p}_{T} (GeV/#it{c})");
    outputContainer->Add(fhPrimEtaAccYeta) ;

    fhPrimEtaPhi    = new TH2F("hPrimEtaPhi","Azimuthal of primary #eta",nptbins,ptmin,ptmax, nphibins,phimin*TMath::RadToDeg(),phimax*TMath::RadToDeg()) ;
    fhPrimEtaPhi->SetYTitle("#phi (deg)");
    fhPrimEtaPhi->SetXTitle("#it{p}_{T} (GeV/#it{c})");
    outputContainer->Add(fhPrimEtaPhi) ;
    
    fhPrimEtaAccPhi = new TH2F("hPrimEtaAccPhi","Azimuthal of primary #eta with accepted daughters",nptbins,ptmin,ptmax, nphibins,phimin*TMath::RadToDeg(),phimax*TMath::RadToDeg()) ;
    fhPrimEtaAccPhi->SetYTitle("#phi (deg)");
    fhPrimEtaAccPhi->SetXTitle("#it{p}_{T} (GeV/#it{c})");
    outputContainer->Add(fhPrimEtaAccPhi) ;
    
    fhPrimEtaPtCentrality     = new TH2F("hPrimEtaPtCentrality","Primary #eta #it{p}_{T} vs reco centrality, |#it{Y}|<1",nptbins,ptmin,ptmax, 100, 0, 100) ;
    fhPrimEtaAccPtCentrality  = new TH2F("hPrimEtaAccPtCentrality","Primary #eta with both photons in acceptance #it{p}_{T} vs reco centrality",nptbins,ptmin,ptmax, 100, 0, 100) ;
    fhPrimEtaPtCentrality   ->SetXTitle("#it{p}_{T} (GeV/#it{c})");
    fhPrimEtaAccPtCentrality->SetXTitle("#it{p}_{T} (GeV/#it{c})");
    fhPrimEtaPtCentrality   ->SetYTitle("Centrality");
    fhPrimEtaAccPtCentrality->SetYTitle("Centrality");
    outputContainer->Add(fhPrimEtaPtCentrality) ;
    outputContainer->Add(fhPrimEtaAccPtCentrality) ;
    
    fhPrimEtaPtEventPlane     = new TH2F("hPrimEtaPtEventPlane","Primary #eta #it{p}_{T} vs reco event plane angle, |#it{Y}|<1",nptbins,ptmin,ptmax, 100, 0, TMath::Pi()) ;
    fhPrimEtaAccPtEventPlane  = new TH2F("hPrimEtaAccPtEventPlane","Primary #eta with both #gamma_{decay} in acceptance #it{p}_{T} vs reco event plane angle",nptbins,ptmin,ptmax, 100, 0, TMath::Pi()) ;
    fhPrimEtaPtEventPlane   ->SetXTitle("#it{p}_{T} (GeV/#it{c})");
    fhPrimEtaAccPtEventPlane->SetXTitle("#it{p}_{T} (GeV/#it{c})");
    fhPrimEtaPtEventPlane   ->SetYTitle("Event Plane Angle (rad)");
    fhPrimEtaAccPtEventPlane->SetYTitle("Event Plane Angle (rad)");
    outputContainer->Add(fhPrimEtaPtEventPlane) ;
    outputContainer->Add(fhPrimEtaAccPtEventPlane) ;
    
     if(fFillAngleHisto)
    {
      fhPrimPi0OpeningAngle  = new TH2F
      ("hPrimPi0OpeningAngle","Angle between all primary #gamma pair vs E_{#pi^{0}}",nptbins,ptmin,ptmax,100,0,0.5); 
      fhPrimPi0OpeningAngle->SetYTitle("#theta(rad)");
      fhPrimPi0OpeningAngle->SetXTitle("E_{ #pi^{0}} (GeV)");
      outputContainer->Add(fhPrimPi0OpeningAngle) ;
      
      fhPrimPi0OpeningAngleAsym  = new TH2F
      ("hPrimPi0OpeningAngleAsym","Angle between all primary #gamma pair vs #it{Asymmetry}, #it{p}_{T}>5 GeV/#it{c}",100,0,1,100,0,0.5);
      fhPrimPi0OpeningAngleAsym->SetXTitle("|A|=| (E_{1}-E_{2}) / (E_{1}+E_{2}) |");
      fhPrimPi0OpeningAngleAsym->SetYTitle("#theta(rad)");
      outputContainer->Add(fhPrimPi0OpeningAngleAsym) ;
      
      fhPrimPi0CosOpeningAngle  = new TH2F
      ("hPrimPi0CosOpeningAngle","Cosinus of angle between all primary #gamma pair vs E_{#pi^{0}}",nptbins,ptmin,ptmax,100,-1,1); 
      fhPrimPi0CosOpeningAngle->SetYTitle("cos (#theta) ");
      fhPrimPi0CosOpeningAngle->SetXTitle("E_{ #pi^{0}} (GeV)");
      outputContainer->Add(fhPrimPi0CosOpeningAngle) ;
      
      fhPrimEtaOpeningAngle  = new TH2F
      ("hPrimEtaOpeningAngle","Angle between all primary #gamma pair vs E_{#eta}",nptbins,ptmin,ptmax,100,0,0.5);
      fhPrimEtaOpeningAngle->SetYTitle("#theta(rad)");
      fhPrimEtaOpeningAngle->SetXTitle("E_{#eta} (GeV)");
      outputContainer->Add(fhPrimEtaOpeningAngle) ;
      
      fhPrimEtaOpeningAngleAsym  = new TH2F
      ("hPrimEtaOpeningAngleAsym","Angle between all primary #gamma pair vs #it{Asymmetry}, #it{p}_{T}>5 GeV/#it{c}",100,0,1,100,0,0.5);
      fhPrimEtaOpeningAngleAsym->SetXTitle("|#it{A}|=| (#it{E}_{1}-#it{E}_{2}) / (#it{E}_{1}+#it{E}_{2}) |");
      fhPrimEtaOpeningAngleAsym->SetYTitle("#theta(rad)");
      outputContainer->Add(fhPrimEtaOpeningAngleAsym) ;

      
      fhPrimEtaCosOpeningAngle  = new TH2F
      ("hPrimEtaCosOpeningAngle","Cosinus of angle between all primary #gamma pair vs E_{#eta}",nptbins,ptmin,ptmax,100,-1,1);
      fhPrimEtaCosOpeningAngle->SetYTitle("cos (#theta) ");
      fhPrimEtaCosOpeningAngle->SetXTitle("#it{E}_{ #eta} (GeV)");
      outputContainer->Add(fhPrimEtaCosOpeningAngle) ;

      
    }
    
    if(fFillOriginHisto)
    {
      //Prim origin
      //Pi0
      fhPrimPi0PtOrigin     = new TH2F("hPrimPi0PtOrigin","Primary #pi^{0} #it{p}_{T} vs origin",nptbins,ptmin,ptmax,11,0,11) ;
      fhPrimPi0PtOrigin->SetXTitle("#it{p}_{T} (GeV/#it{c})");
      fhPrimPi0PtOrigin->SetYTitle("Origin");
      fhPrimPi0PtOrigin->GetYaxis()->SetBinLabel(1 ,"Status 21");
      fhPrimPi0PtOrigin->GetYaxis()->SetBinLabel(2 ,"Quark");
      fhPrimPi0PtOrigin->GetYaxis()->SetBinLabel(3 ,"qq Resonances ");
      fhPrimPi0PtOrigin->GetYaxis()->SetBinLabel(4 ,"Resonances");
      fhPrimPi0PtOrigin->GetYaxis()->SetBinLabel(5 ,"#rho");
      fhPrimPi0PtOrigin->GetYaxis()->SetBinLabel(6 ,"#omega");
      fhPrimPi0PtOrigin->GetYaxis()->SetBinLabel(7 ,"K");
      fhPrimPi0PtOrigin->GetYaxis()->SetBinLabel(8 ,"Other");
      fhPrimPi0PtOrigin->GetYaxis()->SetBinLabel(9 ,"#eta");
      fhPrimPi0PtOrigin->GetYaxis()->SetBinLabel(10 ,"#eta prime");
      outputContainer->Add(fhPrimPi0PtOrigin) ;
      
      fhMCPi0PtOrigin     = new TH2F("hMCPi0PtOrigin","Reconstructed pair from generated #pi^{0} #it{p}_{T} vs origin",nptbins,ptmin,ptmax,11,0,11) ;
      fhMCPi0PtOrigin->SetXTitle("#it{p}_{T} (GeV/#it{c})");
      fhMCPi0PtOrigin->SetYTitle("Origin");
      fhMCPi0PtOrigin->GetYaxis()->SetBinLabel(1 ,"Status 21");
      fhMCPi0PtOrigin->GetYaxis()->SetBinLabel(2 ,"Quark");
      fhMCPi0PtOrigin->GetYaxis()->SetBinLabel(3 ,"qq Resonances");
      fhMCPi0PtOrigin->GetYaxis()->SetBinLabel(4 ,"Resonances");
      fhMCPi0PtOrigin->GetYaxis()->SetBinLabel(5 ,"#rho");
      fhMCPi0PtOrigin->GetYaxis()->SetBinLabel(6 ,"#omega");
      fhMCPi0PtOrigin->GetYaxis()->SetBinLabel(7 ,"K");
      fhMCPi0PtOrigin->GetYaxis()->SetBinLabel(8 ,"Other");
      fhMCPi0PtOrigin->GetYaxis()->SetBinLabel(9 ,"#eta");
      fhMCPi0PtOrigin->GetYaxis()->SetBinLabel(10 ,"#eta prime");
      outputContainer->Add(fhMCPi0PtOrigin) ;    
      
      //Eta
      fhPrimEtaPtOrigin     = new TH2F("hPrimEtaPtOrigin","Primary #pi^{0} #it{p}_{T} vs origin",nptbins,ptmin,ptmax,7,0,7) ;
      fhPrimEtaPtOrigin->SetXTitle("#it{p}_{T} (GeV/#it{c})");
      fhPrimEtaPtOrigin->SetYTitle("Origin");
      fhPrimEtaPtOrigin->GetYaxis()->SetBinLabel(1 ,"Status 21");
      fhPrimEtaPtOrigin->GetYaxis()->SetBinLabel(2 ,"Quark");
      fhPrimEtaPtOrigin->GetYaxis()->SetBinLabel(3 ,"qq Resonances");
      fhPrimEtaPtOrigin->GetYaxis()->SetBinLabel(4 ,"Resonances");
      fhPrimEtaPtOrigin->GetYaxis()->SetBinLabel(5 ,"Other");
      fhPrimEtaPtOrigin->GetYaxis()->SetBinLabel(6 ,"#eta prime ");
      
      outputContainer->Add(fhPrimEtaPtOrigin) ;
      
      fhMCEtaPtOrigin     = new TH2F("hMCEtaPtOrigin","Reconstructed pair from generated #pi^{0} #it{p}_{T} vs origin",nptbins,ptmin,ptmax,7,0,7) ;
      fhMCEtaPtOrigin->SetXTitle("#it{p}_{T} (GeV/#it{c})");
      fhMCEtaPtOrigin->SetYTitle("Origin");
      fhMCEtaPtOrigin->GetYaxis()->SetBinLabel(1 ,"Status 21");
      fhMCEtaPtOrigin->GetYaxis()->SetBinLabel(2 ,"Quark");
      fhMCEtaPtOrigin->GetYaxis()->SetBinLabel(3 ,"qq Resonances");
      fhMCEtaPtOrigin->GetYaxis()->SetBinLabel(4 ,"Resonances");
      fhMCEtaPtOrigin->GetYaxis()->SetBinLabel(5 ,"Other");
      fhMCEtaPtOrigin->GetYaxis()->SetBinLabel(6 ,"#eta prime");
      
      outputContainer->Add(fhMCEtaPtOrigin) ;

      fhMCPi0ProdVertex = new TH2F("hMCPi0ProdVertex","Selected reco pair from generated #pi^{0} #it{p}_{T} vs production vertex",
                                   200,0.,20.,5000,0,500) ;
      fhMCPi0ProdVertex->SetXTitle("#it{p}_{T} (GeV/#it{c})");
      fhMCPi0ProdVertex->SetYTitle("#it{R} (cm)");
      outputContainer->Add(fhMCPi0ProdVertex) ;

      fhMCEtaProdVertex = new TH2F("hMCEtaProdVertex","Selected reco pair from generated #eta #it{p}_{T} vs production vertex",
                                   200,0.,20.,5000,0,500) ;
      fhMCEtaProdVertex->SetXTitle("#it{p}_{T} (GeV/#it{c})");
      fhMCEtaProdVertex->SetYTitle("#it{R} (cm)");
      outputContainer->Add(fhMCEtaProdVertex) ;

      fhPrimPi0ProdVertex = new TH2F("hPrimPi0ProdVertex","generated #pi^{0} #it{p}_{T} vs production vertex",
                                   200,0.,20.,5000,0,500) ;
      fhPrimPi0ProdVertex->SetXTitle("#it{p}_{T} (GeV/#it{c})");
      fhPrimPi0ProdVertex->SetYTitle("#it{R} (cm)");
      outputContainer->Add(fhPrimPi0ProdVertex) ;
      
      fhPrimEtaProdVertex = new TH2F("hPrimEtaProdVertex","generated #eta #it{p}_{T} vs production vertex",
                                   200,0.,20.,5000,0,500) ;
      fhPrimEtaProdVertex->SetXTitle("#it{p}_{T} (GeV/#it{c})");
      fhPrimEtaProdVertex->SetYTitle("#it{R} (cm)");
      outputContainer->Add(fhPrimEtaProdVertex) ;
      
      for(Int_t i = 0; i<13; i++)
      {
        fhMCOrgMass[i] = new TH2F(Form("hMCOrgMass_%d",i),Form("#it{M} vs #it{p}_{T}, origin %d",i),nptbins,ptmin,ptmax,nmassbins,massmin,massmax) ;
        fhMCOrgMass[i]->SetXTitle("#it{p}_{T} (GeV/#it{c})");
        fhMCOrgMass[i]->SetYTitle("#it{M}_{#gamma,#gamma} (GeV/#it{c}^{2})");
        outputContainer->Add(fhMCOrgMass[i]) ;
        
        fhMCOrgAsym[i]= new TH2F(Form("hMCOrgAsym_%d",i),Form("#it{Asymmetry} vs #it{p}_{T}, origin %d",i),nptbins,ptmin,ptmax,nasymbins,asymmin,asymmax) ;
        fhMCOrgAsym[i]->SetXTitle("#it{p}_{T} (GeV/#it{c})");
        fhMCOrgAsym[i]->SetYTitle("A");
        outputContainer->Add(fhMCOrgAsym[i]) ;
        
        fhMCOrgDeltaEta[i] = new TH2F(Form("hMCOrgDeltaEta_%d",i),Form("#Delta #eta of pair vs #it{p}_{T}, origin %d",i),nptbins,ptmin,ptmax,netabins,-1.4,1.4) ;
        fhMCOrgDeltaEta[i]->SetXTitle("#it{p}_{T} (GeV/#it{c})");
        fhMCOrgDeltaEta[i]->SetYTitle("#Delta #eta");
        outputContainer->Add(fhMCOrgDeltaEta[i]) ;
        
        fhMCOrgDeltaPhi[i]= new TH2F(Form("hMCOrgDeltaPhi_%d",i),Form("#Delta #phi of pair vs #it{p}_{T}, origin %d",i),nptbins,ptmin,ptmax,nphibins,-0.7,0.7) ;
        fhMCOrgDeltaPhi[i]->SetXTitle("#it{p}_{T} (GeV/#it{c})");
        fhMCOrgDeltaPhi[i]->SetYTitle("#Delta #phi (rad)");
        outputContainer->Add(fhMCOrgDeltaPhi[i]) ;
        
      }
      
      if(fMultiCutAnaSim)
      {
        fhMCPi0MassPtTrue  = new TH2F*[fNPtCuts*fNAsymCuts*fNCellNCuts];
        fhMCPi0MassPtRec   = new TH2F*[fNPtCuts*fNAsymCuts*fNCellNCuts];
        fhMCPi0PtTruePtRec = new TH2F*[fNPtCuts*fNAsymCuts*fNCellNCuts];
        fhMCEtaMassPtRec   = new TH2F*[fNPtCuts*fNAsymCuts*fNCellNCuts];
        fhMCEtaMassPtTrue  = new TH2F*[fNPtCuts*fNAsymCuts*fNCellNCuts];
        fhMCEtaPtTruePtRec = new TH2F*[fNPtCuts*fNAsymCuts*fNCellNCuts];
        for(Int_t ipt=0; ipt<fNPtCuts; ipt++){
          for(Int_t icell=0; icell<fNCellNCuts; icell++){
            for(Int_t iasym=0; iasym<fNAsymCuts; iasym++){
              Int_t index = ((ipt*fNCellNCuts)+icell)*fNAsymCuts + iasym;
              
              fhMCPi0MassPtRec[index] = new TH2F(Form("hMCPi0MassPtRec_pt%d_cell%d_asym%d",ipt,icell,iasym),
                                                 Form("Reconstructed #it{M} vs reconstructed #it{p}_{T} of true #pi^{0} cluster pairs for #it{p}_{T} >%2.2f, #it{N}^{cluster}_{cell}>%d and |#it{A}|>%1.2f",fPtCuts[ipt],fCellNCuts[icell], fAsymCuts[iasym]),
                                                 nptbins,ptmin,ptmax,nmassbins,massmin,massmax) ;
              fhMCPi0MassPtRec[index]->SetXTitle("#it{p}_{T, reconstructed} (GeV/#it{c})");
              fhMCPi0MassPtRec[index]->SetYTitle("#it{M}_{#gamma,#gamma} (GeV/#it{c}^{2})");
              outputContainer->Add(fhMCPi0MassPtRec[index]) ;    
              
              fhMCPi0MassPtTrue[index] = new TH2F(Form("hMCPi0MassPtTrue_pt%d_cell%d_asym%d",ipt,icell,iasym),
                                                  Form("Reconstructed #it{M} vs generated #it{p}_{T} of true #pi^{0} cluster pairs for #it{p}_{T} >%2.2f, #it{N}^{cluster}_{cell}>%d and |#it{A}|>%1.2f",fPtCuts[ipt],fCellNCuts[icell], fAsymCuts[iasym]),
                                                  nptbins,ptmin,ptmax,nmassbins,massmin,massmax) ;
              fhMCPi0MassPtTrue[index]->SetXTitle("#it{p}_{T, generated} (GeV/#it{c})");
              fhMCPi0MassPtTrue[index]->SetYTitle("#it{M}_{#gamma,#gamma} (GeV/#it{c}^{2})");
              outputContainer->Add(fhMCPi0MassPtTrue[index]) ;
              
              fhMCPi0PtTruePtRec[index] = new TH2F(Form("hMCPi0PtTruePtRec_pt%d_cell%d_asym%d",ipt,icell,iasym),
                                                   Form("Generated vs reconstructed #it{p}_{T} of true #pi^{0} cluster pairs, 0.01 < rec. mass < 0.17 MeV/#it{c}^{2} for #it{p}_{T} >%2.2f, #it{N}^{cluster}_{cell}>%d and |#it{A}|>%1.2f",fPtCuts[ipt],fCellNCuts[icell], fAsymCuts[iasym]),
                                                   nptbins,ptmin,ptmax,nptbins,ptmin,ptmax) ;
              fhMCPi0PtTruePtRec[index]->SetXTitle("#it{p}_{T, generated} (GeV/#it{c})");
              fhMCPi0PtTruePtRec[index]->SetYTitle("#it{p}_{T, reconstructed} (GeV/#it{c})");
              outputContainer->Add(fhMCPi0PtTruePtRec[index]) ;
              
              fhMCEtaMassPtRec[index] = new TH2F(Form("hMCEtaMassPtRec_pt%d_cell%d_asym%d",ipt,icell,iasym),
                                                 Form("Reconstructed #it{M} vs reconstructed #it{p}_{T} of true #eta cluster pairs for #it{p}_{T} >%2.2f, #it{N}^{cluster}_{cell}>%d and |#it{A}|>%1.2f",fPtCuts[ipt],fCellNCuts[icell], fAsymCuts[iasym]),
                                                 nptbins,ptmin,ptmax,nmassbins,massmin,massmax) ;
              fhMCEtaMassPtRec[index]->SetXTitle("#it{p}_{T, generated} (GeV/#it{c})");
              fhMCEtaMassPtRec[index]->SetYTitle("#it{M}_{#gamma,#gamma} (GeV/#it{c}^{2})");
              outputContainer->Add(fhMCEtaMassPtRec[index]) ;
              
              fhMCEtaMassPtTrue[index] = new TH2F(Form("hMCEtaMassPtTrue_pt%d_cell%d_asym%d",ipt,icell,iasym),
                                                  Form("Reconstructed #it{M} vs generated #it{p}_{T} of true #eta cluster pairs for #it{p}_{T} >%2.2f, #it{N}^{cluster}_{cell}>%d and |#it{A}|>%1.2f",fPtCuts[ipt],fCellNCuts[icell], fAsymCuts[iasym]),
                                                  nptbins,ptmin,ptmax,nmassbins,massmin,massmax) ;
              fhMCEtaMassPtTrue[index]->SetXTitle("#it{p}_{T, generated} (GeV/#it{c})");
              fhMCEtaMassPtTrue[index]->SetYTitle("#it{M}_{#gamma,#gamma} (GeV/#it{c}^{2})");
              outputContainer->Add(fhMCEtaMassPtTrue[index]) ;
              
              fhMCEtaPtTruePtRec[index] = new TH2F(Form("hMCEtaPtTruePtRec_pt%d_cell%d_asym%d",ipt,icell,iasym),
                                                   Form("Generated vs reconstructed #it{p}_{T} of true #eta cluster pairs, 0.01 < rec. mass < 0.17 MeV/#it{c}^{2} for #it{p}_{T} >%2.2f, ncell>%d and asym >%1.2f",fPtCuts[ipt],fCellNCuts[icell], fAsymCuts[iasym]),
                                                   nptbins,ptmin,ptmax,nptbins,ptmin,ptmax) ;
              fhMCEtaPtTruePtRec[index]->SetXTitle("#it{p}_{T, generated} (GeV/#it{c})");
              fhMCEtaPtTruePtRec[index]->SetYTitle("#it{p}_{T, reconstructed} (GeV/#it{c})");
              outputContainer->Add(fhMCEtaPtTruePtRec[index]) ;
            }
          }
        }  
      }//multi cut ana
      else
      {
        fhMCPi0MassPtTrue  = new TH2F*[1];
        fhMCPi0PtTruePtRec = new TH2F*[1];
        fhMCEtaMassPtTrue  = new TH2F*[1];
        fhMCEtaPtTruePtRec = new TH2F*[1];
        
        fhMCPi0MassPtTrue[0] = new TH2F("hMCPi0MassPtTrue","Reconstructed Mass vs generated p_T of true #pi^{0} cluster pairs",nptbins,ptmin,ptmax,nmassbins,massmin,massmax) ;
        fhMCPi0MassPtTrue[0]->SetXTitle("#it{p}_{T, generated} (GeV/#it{c})");
        fhMCPi0MassPtTrue[0]->SetYTitle("#it{M}_{#gamma,#gamma} (GeV/#it{c}^{2})");
        outputContainer->Add(fhMCPi0MassPtTrue[0]) ;
        
        fhMCPi0PtTruePtRec[0]= new TH2F("hMCPi0PtTruePtRec","Generated vs reconstructed p_T of true #pi^{0} cluster pairs, 0.01 < rec. mass < 0.17 MeV/#it{c}^{2}",nptbins,ptmin,ptmax,nptbins,ptmin,ptmax) ;
        fhMCPi0PtTruePtRec[0]->SetXTitle("#it{p}_{T, generated} (GeV/#it{c})");
        fhMCPi0PtTruePtRec[0]->SetYTitle("#it{p}_{T, reconstructed} (GeV/#it{c})");
        outputContainer->Add(fhMCPi0PtTruePtRec[0]) ;
        
        fhMCEtaMassPtTrue[0] = new TH2F("hMCEtaMassPtTrue","Reconstructed Mass vs generated p_T of true #eta cluster pairs",nptbins,ptmin,ptmax,nmassbins,massmin,massmax) ;
        fhMCEtaMassPtTrue[0]->SetXTitle("#it{p}_{T, generated} (GeV/#it{c})");
        fhMCEtaMassPtTrue[0]->SetYTitle("#it{M}_{#gamma,#gamma} (GeV/#it{c}^{2})");
        outputContainer->Add(fhMCEtaMassPtTrue[0]) ;
        
        fhMCEtaPtTruePtRec[0]= new TH2F("hMCEtaPtTruePtRec","Generated vs reconstructed p_T of true #eta cluster pairs, 0.01 < rec. mass < 0.17 MeV/#it{c}^{2}",nptbins,ptmin,ptmax,nptbins,ptmin,ptmax) ;
        fhMCEtaPtTruePtRec[0]->SetXTitle("#it{p}_{T, generated} (GeV/#it{c})");
        fhMCEtaPtTruePtRec[0]->SetYTitle("#it{p}_{T, reconstructed} (GeV/#it{c})");
        outputContainer->Add(fhMCEtaPtTruePtRec[0]) ;
      }
    }
  }
  
  if(fFillSMCombinations)
  {
    TString pairnamePHOS[] = {"(0-1)","(0-2)","(1-2)","(0-3)","(0-4)","(1-3)","(1-4)","(2-3)","(2-4)","(3-4)"};
    for(Int_t imod=0; imod<fNModules; imod++)
    {
      //Module dependent invariant mass
      snprintf(key, buffersize,"hReMod_%d",imod) ;
      snprintf(title, buffersize,"Real #it{M}_{#gamma#gamma} distr. for Module %d",imod) ;
      fhReMod[imod]  = new TH2F(key,title,nptbins,ptmin,ptmax,nmassbins,massmin,massmax) ;
      fhReMod[imod]->SetXTitle("#it{p}_{T} (GeV/#it{c})");
      fhReMod[imod]->SetYTitle("#it{M}_{#gamma,#gamma} (GeV/#it{c}^{2})");
      outputContainer->Add(fhReMod[imod]) ;
      if(fCalorimeter=="PHOS")
      {
        snprintf(key, buffersize,"hReDiffPHOSMod_%d",imod) ;
        snprintf(title, buffersize,"Real pairs PHOS, clusters in different Modules: %s",(pairnamePHOS[imod]).Data()) ;
        fhReDiffPHOSMod[imod]  = new TH2F(key,title,nptbins,ptmin,ptmax,nmassbins,massmin,massmax) ;
        fhReDiffPHOSMod[imod]->SetXTitle("#it{p}_{T} (GeV/#it{c})");
        fhReDiffPHOSMod[imod]->SetYTitle("#it{M}_{#gamma,#gamma} (GeV/#it{c}^{2})");
        outputContainer->Add(fhReDiffPHOSMod[imod]) ;
      }
      else{//EMCAL
        if(imod<fNModules/2)
        {
          snprintf(key, buffersize,"hReSameSectorEMCAL_%d",imod) ;
          snprintf(title, buffersize,"Real pairs EMCAL, clusters in same sector, SM(%d,%d)",imod*2,imod*2+1) ;
          fhReSameSectorEMCALMod[imod]  = new TH2F(key,title,nptbins,ptmin,ptmax,nmassbins,massmin,massmax) ;
          fhReSameSectorEMCALMod[imod]->SetXTitle("#it{p}_{T} (GeV/#it{c})");
          fhReSameSectorEMCALMod[imod]->SetYTitle("#it{M}_{#gamma,#gamma} (GeV/#it{c}^{2})");
          outputContainer->Add(fhReSameSectorEMCALMod[imod]) ;
        }
        if(imod<fNModules-2)
        {
          snprintf(key, buffersize,"hReSameSideEMCAL_%d",imod) ;
          snprintf(title, buffersize,"Real pairs EMCAL, clusters in same side SM(%d,%d)",imod, imod+2) ;
          fhReSameSideEMCALMod[imod]  = new TH2F(key,title,nptbins,ptmin,ptmax,nmassbins,massmin,massmax) ;
          fhReSameSideEMCALMod[imod]->SetXTitle("#it{p}_{T} (GeV/#it{c})");
          fhReSameSideEMCALMod[imod]->SetYTitle("#it{M}_{#gamma,#gamma} (GeV/#it{c}^{2})");
          outputContainer->Add(fhReSameSideEMCALMod[imod]) ;
        }
      }//EMCAL
      
      if(DoOwnMix())
      { 
        snprintf(key, buffersize,"hMiMod_%d",imod) ;
        snprintf(title, buffersize,"Mixed #it{M}_{#gamma#gamma} distr. for Module %d",imod) ;
        fhMiMod[imod]  = new TH2F(key,title,nptbins,ptmin,ptmax,nmassbins,massmin,massmax) ;
        fhMiMod[imod]->SetXTitle("#it{p}_{T} (GeV/#it{c})");
        fhMiMod[imod]->SetYTitle("#it{M}_{#gamma,#gamma} (GeV/#it{c}^{2})");
        outputContainer->Add(fhMiMod[imod]) ;
        
        if(fCalorimeter=="PHOS"){
          snprintf(key, buffersize,"hMiDiffPHOSMod_%d",imod) ;
          snprintf(title, buffersize,"Mixed pairs PHOS, clusters in different Modules: %s",(pairnamePHOS[imod]).Data()) ;
          fhMiDiffPHOSMod[imod]  = new TH2F(key,title,nptbins,ptmin,ptmax,nmassbins,massmin,massmax) ;
          fhMiDiffPHOSMod[imod]->SetXTitle("#it{p}_{T} (GeV/#it{c})");
          fhMiDiffPHOSMod[imod]->SetYTitle("#it{M}_{#gamma,#gamma} (GeV/#it{c}^{2})");
          outputContainer->Add(fhMiDiffPHOSMod[imod]) ;
        }//PHOS
        else{//EMCAL
          if(imod<fNModules/2)
          {
            snprintf(key, buffersize,"hMiSameSectorEMCALMod_%d",imod) ;
            snprintf(title, buffersize,"Mixed pairs EMCAL, clusters in same sector, SM(%d,%d)",imod*2,imod*2+1) ;
            fhMiSameSectorEMCALMod[imod]  = new TH2F(key,title,nptbins,ptmin,ptmax,nmassbins,massmin,massmax) ;
            fhMiSameSectorEMCALMod[imod]->SetXTitle("#it{p}_{T} (GeV/#it{c})");
            fhMiSameSectorEMCALMod[imod]->SetYTitle("#it{M}_{#gamma,#gamma} (GeV/#it{c}^{2})");
            outputContainer->Add(fhMiSameSectorEMCALMod[imod]) ;
          }
          if(imod<fNModules-2){
            
            snprintf(key, buffersize,"hMiSameSideEMCALMod_%d",imod) ;
            snprintf(title, buffersize,"Mixed pairs EMCAL, clusters in same side SM(%d,%d)",imod, imod+2) ;
            fhMiSameSideEMCALMod[imod]  = new TH2F(key,title,nptbins,ptmin,ptmax,nmassbins,massmin,massmax) ;
            fhMiSameSideEMCALMod[imod]->SetXTitle("#it{p}_{T} (GeV/#it{c})");
            fhMiSameSideEMCALMod[imod]->SetYTitle("#it{M}_{#gamma,#gamma} (GeV/#it{c}^{2})");
            outputContainer->Add(fhMiSameSideEMCALMod[imod]) ;
          }
        }//EMCAL      
      }// own mix
    }//loop combinations
  } // SM combinations
  
  if(fFillArmenterosThetaStar && IsDataMC())
  {
    TString ebin[] = {"8 < E < 12 GeV","12 < E < 16 GeV", "16 < E < 20 GeV", "E > 20 GeV" };
    Int_t narmbins = 400;
    Float_t armmin = 0;
    Float_t armmax = 0.4;
    
    for(Int_t i = 0; i < 4; i++)
    {
      fhArmPrimPi0[i] =  new TH2F(Form("hArmenterosPrimPi0EBin%d",i),
                                  Form("Armenteros of primary #pi^{0}, %s",ebin[i].Data()),
                                  200, -1, 1, narmbins,armmin,armmax);
      fhArmPrimPi0[i]->SetYTitle("#it{p}_{T}^{Arm}");
      fhArmPrimPi0[i]->SetXTitle("#alpha^{Arm}");
      outputContainer->Add(fhArmPrimPi0[i]) ;

      fhArmPrimEta[i] =  new TH2F(Form("hArmenterosPrimEtaEBin%d",i),
                                      Form("Armenteros of primary #eta, %s",ebin[i].Data()),
                                      200, -1, 1, narmbins,armmin,armmax);
      fhArmPrimEta[i]->SetYTitle("#it{p}_{T}^{Arm}");
      fhArmPrimEta[i]->SetXTitle("#alpha^{Arm}");
      outputContainer->Add(fhArmPrimEta[i]) ;
      
    }
    
    // Same as asymmetry ...
    fhCosThStarPrimPi0  = new TH2F
    ("hCosThStarPrimPi0","cos(#theta *) for primary #pi^{0}",nptbins,ptmin,ptmax,200,-1,1);
    fhCosThStarPrimPi0->SetYTitle("cos(#theta *)");
    fhCosThStarPrimPi0->SetXTitle("E_{ #pi^{0}} (GeV)");
    outputContainer->Add(fhCosThStarPrimPi0) ;
    
    fhCosThStarPrimEta  = new TH2F
    ("hCosThStarPrimEta","cos(#theta *) for primary #eta",nptbins,ptmin,ptmax,200,-1,1);
    fhCosThStarPrimEta->SetYTitle("cos(#theta *)");
    fhCosThStarPrimEta->SetXTitle("E_{ #eta} (GeV)");
    outputContainer->Add(fhCosThStarPrimEta) ;
    
  }
  
  //  for(Int_t i = 0; i < outputContainer->GetEntries() ; i++){
  //  
  //    printf("Histogram %d, name: %s\n ",i, outputContainer->At(i)->GetName());
  //  
  //  }
  
  return outputContainer;
}

//___________________________________________________
void AliAnaPi0::Print(const Option_t * /*opt*/) const
{
  //Print some relevant parameters set for the analysis
  printf("**** Print %s %s ****\n", GetName(), GetTitle() ) ;
  AliAnaCaloTrackCorrBaseClass::Print(" ");
  
  printf("Number of bins in Centrality:  %d \n",GetNCentrBin()) ;
  printf("Number of bins in Z vert. pos: %d \n",GetNZvertBin()) ;
  printf("Number of bins in Reac. Plain: %d \n",GetNRPBin()) ;
  printf("Depth of event buffer: %d \n",GetNMaxEvMix()) ;
  printf("Pair in same Module: %d \n",fSameSM) ;
  printf("Cuts: \n") ;
  // printf("Z vertex position: -%2.3f < z < %2.3f \n",GetZvertexCut(),GetZvertexCut()) ; //It crashes here, why?
  printf("Number of modules:             %d \n",fNModules) ;
  printf("Select pairs with their angle: %d, edep %d, min angle %2.3f, max angle %2.3f \n",fUseAngleCut, fUseAngleEDepCut, fAngleCut, fAngleMaxCut) ;
  printf("Asymmetry cuts: n = %d, \n",fNAsymCuts) ;
  printf("\tasymmetry < ");
  for(Int_t i = 0; i < fNAsymCuts; i++) printf("%2.2f ",fAsymCuts[i]);
  printf("\n");
  
  printf("PID selection bits: n = %d, \n",fNPIDBits) ;
  printf("\tPID bit = ");
  for(Int_t i = 0; i < fNPIDBits; i++) printf("%d ",fPIDBits[i]);
  printf("\n");
  
  if(fMultiCutAna){
    printf("pT cuts: n = %d, \n",fNPtCuts) ;
    printf("\tpT > ");
    for(Int_t i = 0; i < fNPtCuts; i++) printf("%2.2f ",fPtCuts[i]);
    printf("GeV/c\n");
    
    printf("N cell in cluster cuts: n = %d, \n",fNCellNCuts) ;
    printf("\tnCell > ");
    for(Int_t i = 0; i < fNCellNCuts; i++) printf("%d ",fCellNCuts[i]);
    printf("\n");
    
  }
  printf("------------------------------------------------------\n") ;
} 

//________________________________________
void AliAnaPi0::FillAcceptanceHistograms()
{
  //Fill acceptance histograms if MC data is available
  
  Double_t mesonY   = -100 ;
  Double_t mesonE   = -1 ;
  Double_t mesonPt  = -1 ;
  Double_t mesonPhi =  100 ;
  Double_t mesonYeta= -1 ;
  
  Int_t    pdg     = 0 ;
  Int_t    nprim   = 0 ;
  Int_t    nDaught = 0 ;
  Int_t    iphot1  = 0 ;
  Int_t    iphot2  = 0 ;
  
  Float_t cen = GetEventCentrality();
  Float_t ep  = GetEventPlaneAngle();
  
  TParticle        * primStack = 0;
  AliAODMCParticle * primAOD   = 0;
  TLorentzVector lvmeson;
  
  // Get the ESD MC particles container
  AliStack * stack = 0;
  if( GetReader()->ReadStack() )
  {
    stack = GetMCStack();
    if(!stack ) return;
    nprim = stack->GetNtrack();
  }
  
  // Get the AOD MC particles container
  TClonesArray * mcparticles = 0;
  if( GetReader()->ReadAODMCParticles() )
  {
    mcparticles = GetReader()->GetAODMCParticles();
    if( !mcparticles ) return;
    nprim = mcparticles->GetEntriesFast();
  }
  
  for(Int_t i=0 ; i < nprim; i++)
  {
    if(GetReader()->AcceptOnlyHIJINGLabels() && !GetReader()->IsHIJINGLabel(i)) continue ;
    
    if(GetReader()->ReadStack())
    {
      primStack = stack->Particle(i) ;
      if(!primStack)
      {
        printf("AliAnaPi0::FillAcceptanceHistograms() - ESD primaries pointer not available!!\n");
        continue;
      }
      
      // If too small  skip
      if( primStack->Energy() < 0.4 ) continue;

      pdg       = primStack->GetPdgCode();
      nDaught   = primStack->GetNDaughters();
      iphot1    = primStack->GetDaughter(0) ;
      iphot2    = primStack->GetDaughter(1) ;
      if(primStack->Energy() == TMath::Abs(primStack->Pz()))  continue ; //Protection against floating point exception
      
      //printf("i %d, %s %d  %s %d \n",i, stack->Particle(i)->GetName(), stack->Particle(i)->GetPdgCode(),
      //       prim->GetName(), prim->GetPdgCode());
      
      //Photon kinematics
      primStack->Momentum(lvmeson);
      
      mesonY = 0.5*TMath::Log((primStack->Energy()-primStack->Pz())/(primStack->Energy()+primStack->Pz())) ;
    }
    else
    {
      primAOD = (AliAODMCParticle *) mcparticles->At(i);
      if(!primAOD)
      {
        printf("AliAnaPi0::FillAcceptanceHistograms() - AOD primaries pointer not available!!\n");
        continue;
      }
      
      // If too small  skip
      if( primAOD->E() < 0.4 ) continue;
      
      pdg     = primAOD->GetPdgCode();
      nDaught = primAOD->GetNDaughters();
      iphot1  = primAOD->GetFirstDaughter() ;
      iphot2  = primAOD->GetLastDaughter() ;
      
      if(primAOD->E() == TMath::Abs(primAOD->Pz()))  continue ; //Protection against floating point exception
      
      //Photon kinematics
      lvmeson.SetPxPyPzE(primAOD->Px(),primAOD->Py(),primAOD->Pz(),primAOD->E());
      
      mesonY = 0.5*TMath::Log((primAOD->E()-primAOD->Pz())/(primAOD->E()+primAOD->Pz())) ;
    }
    
    // Select only pi0 or eta
    if( pdg != 111 && pdg != 221) continue ;
    
    mesonPt  = lvmeson.Pt () ;
    mesonE   = lvmeson.E  () ;
    mesonYeta= lvmeson.Eta() ;
    mesonPhi = lvmeson.Phi() ;
    if( mesonPhi < 0 ) mesonPhi+=TMath::TwoPi();
    mesonPhi *= TMath::RadToDeg();
    
    if(pdg == 111)
    {
      if(TMath::Abs(mesonY) < 1.0)
      {
        fhPrimPi0E  ->Fill(mesonE ) ;
        fhPrimPi0Pt ->Fill(mesonPt) ;
        fhPrimPi0Phi->Fill(mesonPt, mesonPhi) ;
        
        fhPrimPi0YetaYcut    ->Fill(mesonPt,mesonYeta) ;
        fhPrimPi0PtCentrality->Fill(mesonPt,cen) ;
        fhPrimPi0PtEventPlane->Fill(mesonPt,ep ) ;
      }
      
      fhPrimPi0Y   ->Fill(mesonPt, mesonY) ;
      fhPrimPi0Yeta->Fill(mesonPt, mesonYeta) ;
    }
    else if(pdg == 221)
    {
      if(TMath::Abs(mesonY) < 1.0)
      {
        fhPrimEtaE  ->Fill(mesonE ) ;
        fhPrimEtaPt ->Fill(mesonPt) ;
        fhPrimEtaPhi->Fill(mesonPt, mesonPhi) ;
        
        fhPrimEtaYetaYcut    ->Fill(mesonPt,mesonYeta) ;
        fhPrimEtaPtCentrality->Fill(mesonPt,cen) ;
        fhPrimEtaPtEventPlane->Fill(mesonPt,ep ) ;
      }
      
      fhPrimEtaY   ->Fill(mesonPt, mesonY) ;
      fhPrimEtaYeta->Fill(mesonPt, mesonYeta) ;
    }
    
    //Origin of meson
    if(fFillOriginHisto && TMath::Abs(mesonY) < 0.7)
    {
      Int_t momindex  = -1;
      Int_t mompdg    = -1;
      Int_t momstatus = -1;
      Float_t momR    =  0;
      if(GetReader()->ReadStack())          momindex = primStack->GetFirstMother();
      if(GetReader()->ReadAODMCParticles()) momindex = primAOD  ->GetMother();
      
      if(momindex >= 0 && momindex < nprim)
      {
        if(GetReader()->ReadStack())
        {
          TParticle* mother = stack->Particle(momindex);
          mompdg    = TMath::Abs(mother->GetPdgCode());
          momstatus = mother->GetStatusCode();
          momR      = mother->R();
        }
        
        if(GetReader()->ReadAODMCParticles())
        {
          AliAODMCParticle* mother = (AliAODMCParticle*) mcparticles->At(momindex);
          mompdg    = TMath::Abs(mother->GetPdgCode());
          momstatus = mother->GetStatus();
          momR      = TMath::Sqrt(mother->Xv()*mother->Xv()+mother->Yv()*mother->Yv());
        }
        
        if(pdg == 111)
        {
          if     (momstatus  == 21)fhPrimPi0PtOrigin->Fill(mesonPt,0.5);//parton
          else if(mompdg     < 22 ) fhPrimPi0PtOrigin->Fill(mesonPt,1.5);//quark
          else if(mompdg     > 2100  && mompdg   < 2210) fhPrimPi0PtOrigin->Fill(mesonPt,2.5);// resonances
          else if(mompdg    == 221) fhPrimPi0PtOrigin->Fill(mesonPt,8.5);//eta
          else if(mompdg    == 331) fhPrimPi0PtOrigin->Fill(mesonPt,9.5);//eta prime
          else if(mompdg    == 213) fhPrimPi0PtOrigin->Fill(mesonPt,4.5);//rho
          else if(mompdg    == 223) fhPrimPi0PtOrigin->Fill(mesonPt,5.5);//omega
          else if(mompdg    >= 310   && mompdg    <= 323) fhPrimPi0PtOrigin->Fill(mesonPt,6.5);//k0S, k+-,k*
          else if(mompdg    == 130) fhPrimPi0PtOrigin->Fill(mesonPt,6.5);//k0L
          else if(momstatus == 11 || momstatus  == 12 ) fhPrimPi0PtOrigin->Fill(mesonPt,3.5);//resonances
          else                      fhPrimPi0PtOrigin->Fill(mesonPt,7.5);//other?
          
          //printf("Prim Pi0: index %d, pt %2.2f Prod vertex %3.3f, origin pdg %d, origin status %d, origin UI %d\n",
          //                   momindex, mesonPt,mother->R(),mompdg,momstatus,mother->GetUniqueID());
          
          fhPrimPi0ProdVertex->Fill(mesonPt,momR);
          
        }//pi0
        else
        {
          if     (momstatus == 21 ) fhPrimEtaPtOrigin->Fill(mesonPt,0.5);//parton
          else if(mompdg    < 22  ) fhPrimEtaPtOrigin->Fill(mesonPt,1.5);//quark
          else if(mompdg    > 2100  && mompdg   < 2210) fhPrimEtaPtOrigin->Fill(mesonPt,2.5);//qq resonances
          else if(mompdg    == 331) fhPrimEtaPtOrigin->Fill(mesonPt,5.5);//eta prime
          else if(momstatus == 11 || momstatus  == 12 ) fhPrimEtaPtOrigin->Fill(mesonPt,3.5);//resonances
          else fhPrimEtaPtOrigin->Fill(mesonPt,4.5);//stable, conversions?
          //printf("Other Meson pdg %d, Mother %s, pdg %d, status %d\n",pdg, TDatabasePDG::Instance()->GetParticle(mompdg)->GetName(),mompdg, momstatus );
          
          fhPrimEtaProdVertex->Fill(mesonPt,momR);
          
        }
      } // pi0 has mother
    }
    
    //Check if both photons hit Calorimeter
    if(nDaught != 2 ) continue; //Only interested in 2 gamma decay
    
    if(iphot1 < 0 || iphot1 >= nprim || iphot2 < 0 || iphot2 >= nprim) continue ;
    
    TLorentzVector lv1, lv2;
    Int_t pdg1 = 0;
    Int_t pdg2 = 0;
    Bool_t inacceptance1 = kTRUE;
    Bool_t inacceptance2 = kTRUE;
    
    if(GetReader()->ReadStack())
    {
      TParticle * phot1 = stack->Particle(iphot1) ;
      TParticle * phot2 = stack->Particle(iphot2) ;
      
      if(!phot1 || !phot2) continue ;
      
      pdg1 = phot1->GetPdgCode();
      pdg2 = phot2->GetPdgCode();
      
      phot1->Momentum(lv1);
      phot2->Momentum(lv2);
      
      // Check if photons hit the Calorimeter acceptance
      if(IsRealCaloAcceptanceOn())
      {
        if( !GetCaloUtils()->IsMCParticleInCalorimeterAcceptance( fCalorimeter, phot1 )) inacceptance1 = kFALSE ;
        if( !GetCaloUtils()->IsMCParticleInCalorimeterAcceptance( fCalorimeter, phot2 )) inacceptance2 = kFALSE ;
      }
    }
    
    if(GetReader()->ReadAODMCParticles())
    {
      AliAODMCParticle * phot1 = (AliAODMCParticle *) mcparticles->At(iphot1) ;
      AliAODMCParticle * phot2 = (AliAODMCParticle *) mcparticles->At(iphot2) ;
      
      if(!phot1 || !phot2) continue ;
      
      pdg1 = phot1->GetPdgCode();
      pdg2 = phot2->GetPdgCode();
      
      lv1.SetPxPyPzE(phot1->Px(),phot1->Py(),phot1->Pz(),phot1->E());
      lv2.SetPxPyPzE(phot2->Px(),phot2->Py(),phot2->Pz(),phot2->E());
      
      // Check if photons hit the Calorimeter acceptance
      if(IsRealCaloAcceptanceOn())
      {
        if( !GetCaloUtils()->IsMCParticleInCalorimeterAcceptance( fCalorimeter, phot1 )) inacceptance1 = kFALSE ;
        if( !GetCaloUtils()->IsMCParticleInCalorimeterAcceptance( fCalorimeter, phot2 )) inacceptance2 = kFALSE ;
      }
    }
    
    if( pdg1 != 22 || pdg2 !=22) continue ;
    
    // Check if photons hit desired acceptance in the fidutial borders fixed in the analysis
    if(IsFiducialCutOn())
    {
      if( inacceptance1 && !GetFiducialCut()->IsInFiducialCut(lv1,fCalorimeter) ) inacceptance1 = kFALSE ;
      if( inacceptance2 && !GetFiducialCut()->IsInFiducialCut(lv2,fCalorimeter) ) inacceptance2 = kFALSE ;
    }
    
    if(fFillArmenterosThetaStar) FillArmenterosThetaStar(pdg,lvmeson,lv1,lv2);

    if(fCalorimeter=="EMCAL" && inacceptance1 && inacceptance2 && fCheckAccInSector)
    {
      Int_t absID1=0;
      Int_t absID2=0;
      
      Float_t photonPhi1 = lv1.Phi();
      Float_t photonPhi2 = lv2.Phi();
      
      if(photonPhi1 < 0) photonPhi1+=TMath::TwoPi();
      if(photonPhi2 < 0) photonPhi2+=TMath::TwoPi();
      
      GetEMCALGeometry()->GetAbsCellIdFromEtaPhi(lv1.Eta(),photonPhi1,absID1);
      GetEMCALGeometry()->GetAbsCellIdFromEtaPhi(lv2.Eta(),photonPhi2,absID2);
      
      Int_t sm1 = GetEMCALGeometry()->GetSuperModuleNumber(absID1);
      Int_t sm2 = GetEMCALGeometry()->GetSuperModuleNumber(absID2);
      
      Int_t j=0;
      Bool_t sameSector = kFALSE;
      for(Int_t isector = 0; isector < fNModules/2; isector++)
      {
        j=2*isector;
        if((sm1==j && sm2==j+1) || (sm1==j+1 && sm2==j)) sameSector = kTRUE;
      }
      
      if(sm1!=sm2 && !sameSector)
      {
        inacceptance1 = kFALSE;
        inacceptance2 = kFALSE;
      }
      //if(sm1!=sm2)printf("sm1 %d, sm2 %d, same sector %d, in acceptance %d\n",sm1,sm2,sameSector,inacceptance);
      //                  if(GetEMCALGeometry()->Impact(phot1) && GetEMCALGeometry()->Impact(phot2))
      //                    inacceptance = kTRUE;
    }
    
    if(GetDebug() > 2)
      printf("Accepted in %s?: m (%2.2f,%2.2f,%2.2f), p1 (%2.2f,%2.2f,%2.2f), p2 (%2.2f,%2.2f,%2.2f) : in1 %d, in2 %d\n",
             fCalorimeter.Data(),
             mesonPt,mesonYeta,mesonPhi,
             lv1.Pt(),lv1.Eta(),lv1.Phi()*TMath::RadToDeg(),
             lv2.Pt(),lv2.Eta(),lv2.Phi()*TMath::RadToDeg(),
             inacceptance1, inacceptance2);

    
    if(inacceptance1 && inacceptance2)
    {
      Float_t  asym  = TMath::Abs((lv1.E()-lv2.E()) / (lv1.E()+lv2.E()));
      Double_t angle = lv1.Angle(lv2.Vect());
      
      if(GetDebug() > 2)
        printf("\t ACCEPTED pdg %d: pt %2.2f, phi %2.2f, eta %2.2f\n",pdg,mesonPt,mesonPhi,mesonYeta);
      
      if(pdg==111)
      {
        fhPrimPi0AccE   ->Fill(mesonE) ;
        fhPrimPi0AccPt  ->Fill(mesonPt) ;
        fhPrimPi0AccPhi ->Fill(mesonPt, mesonPhi) ;
        fhPrimPi0AccY   ->Fill(mesonPt, mesonY) ;
        fhPrimPi0AccYeta->Fill(mesonPt, mesonYeta) ;
        fhPrimPi0AccPtCentrality->Fill(mesonPt,cen) ;
        fhPrimPi0AccPtEventPlane->Fill(mesonPt,ep ) ;
        
        if(fFillAngleHisto)
        {
          fhPrimPi0OpeningAngle    ->Fill(mesonPt,angle);
          if(mesonPt > 5)fhPrimPi0OpeningAngleAsym->Fill(asym,angle);
          fhPrimPi0CosOpeningAngle ->Fill(mesonPt,TMath::Cos(angle));
        }
      }
      else if(pdg==221)
      {
        fhPrimEtaAccE   ->Fill(mesonE ) ;
        fhPrimEtaAccPt  ->Fill(mesonPt) ;
        fhPrimEtaAccPhi ->Fill(mesonPt, mesonPhi) ;
        fhPrimEtaAccY   ->Fill(mesonPt, mesonY) ;
        fhPrimEtaAccYeta->Fill(mesonPt, mesonYeta) ;
        fhPrimEtaAccPtCentrality->Fill(mesonPt,cen) ;
        fhPrimEtaAccPtEventPlane->Fill(mesonPt,ep ) ;
        
        if(fFillAngleHisto)
        {
          fhPrimEtaOpeningAngle    ->Fill(mesonPt,angle);
          if(mesonPt > 5)fhPrimEtaOpeningAngleAsym->Fill(asym,angle);
          fhPrimEtaCosOpeningAngle ->Fill(mesonPt,TMath::Cos(angle));
        }
      }
    }//Accepted
    
  }//loop on primaries
  
}

//__________________________________________________________________________________
void AliAnaPi0::FillArmenterosThetaStar(Int_t pdg,             TLorentzVector meson,
                                        TLorentzVector daugh1, TLorentzVector daugh2)
{
  // Fill armenteros plots
  
  // Get pTArm and AlphaArm
  Float_t momentumSquaredMother = meson.P()*meson.P();
  Float_t momentumDaughter1AlongMother = 0.;
  Float_t momentumDaughter2AlongMother = 0.;
  
  if (momentumSquaredMother > 0.)
  {
    momentumDaughter1AlongMother = (daugh1.Px()*meson.Px() + daugh1.Py()*meson.Py()+ daugh1.Pz()*meson.Pz()) / sqrt(momentumSquaredMother);
    momentumDaughter2AlongMother = (daugh2.Px()*meson.Px() + daugh2.Py()*meson.Py()+ daugh2.Pz()*meson.Pz()) / sqrt(momentumSquaredMother);
  }
  
  Float_t momentumSquaredDaughter1 = daugh1.P()*daugh1.P();
  Float_t ptArmSquared = momentumSquaredDaughter1 - momentumDaughter1AlongMother*momentumDaughter1AlongMother;
  
  Float_t pTArm = 0.;
  if (ptArmSquared > 0.)
    pTArm = sqrt(ptArmSquared);
  
  Float_t alphaArm = 0.;
  if(momentumDaughter1AlongMother +momentumDaughter2AlongMother > 0)
    alphaArm = (momentumDaughter1AlongMother -momentumDaughter2AlongMother) / (momentumDaughter1AlongMother + momentumDaughter2AlongMother);
  
  TLorentzVector daugh1Boost = daugh1;
  daugh1Boost.Boost(-meson.BoostVector());
  Float_t  cosThStar=TMath::Cos(daugh1Boost.Vect().Angle(meson.Vect()));
  
  Float_t en   = meson.Energy();
  Int_t   ebin = -1;
  if(en > 8  && en <= 12) ebin = 0;
  if(en > 12 && en <= 16) ebin = 1;
  if(en > 16 && en <= 20) ebin = 2;
  if(en > 20)             ebin = 3;
  if(ebin < 0 || ebin > 3) return ;
  
  
  if(pdg==111)
  {
    fhCosThStarPrimPi0->Fill(en,cosThStar);
    fhArmPrimPi0[ebin]->Fill(alphaArm,pTArm);
  }
  else
  {
    fhCosThStarPrimEta->Fill(en,cosThStar);
    fhArmPrimEta[ebin]->Fill(alphaArm,pTArm);
  }
  
  if(GetDebug() > 2 )
  {
    Float_t asym = 0;
    if(daugh1.E()+daugh2.E() > 0) asym = TMath::Abs(daugh1.E()-daugh2.E())/(daugh1.E()+daugh2.E());

    printf("AliAnaPi0::FillArmenterosThetaStar() - E %f, alphaArm %f, pTArm %f, cos(theta*) %f, asymmetry %1.3f\n",
         en,alphaArm,pTArm,cosThStar,asym);
  }
}

//_______________________________________________________________________________________
void AliAnaPi0::FillMCVersusRecDataHistograms(Int_t index1,  Int_t index2,
                                              Float_t pt1,   Float_t pt2,
                                              Int_t ncell1,  Int_t ncell2,
                                              Double_t mass, Double_t pt,  Double_t asym,
                                              Double_t deta, Double_t dphi)
{
  //Do some MC checks on the origin of the pair, is there any common ancestor and if there is one, who?
  //Adjusted for Pythia, need to see what to do for other generators.
  //Array of histograms ordered as follows: 0-Photon, 1-electron, 2-pi0, 3-eta, 4-a-proton, 5-a-neutron, 6-stable particles,
  // 7-other decays, 8-string, 9-final parton, 10-initial parton, intermediate, 11-colliding proton, 12-unrelated
  
  Int_t ancPDG    = 0;
  Int_t ancStatus = 0;
  TLorentzVector ancMomentum;
  TVector3 prodVertex;
  Int_t ancLabel  = GetMCAnalysisUtils()->CheckCommonAncestor(index1, index2,
                                                              GetReader(), ancPDG, ancStatus,ancMomentum, prodVertex);
  
  Int_t momindex  = -1;
  Int_t mompdg    = -1;
  Int_t momstatus = -1;
  if(GetDebug() > 1) printf("AliAnaPi0::FillMCVersusRecDataHistograms() - Common ancestor label %d, pdg %d, name %s, status %d; \n",
                            ancLabel,ancPDG,TDatabasePDG::Instance()->GetParticle(ancPDG)->GetName(),ancStatus);
  
  Float_t prodR = -1;
  Int_t mcIndex = -1;
  
  if(ancLabel > -1)
  {
    if(ancPDG==22)
    {//gamma
      mcIndex = 0;
    }
    else if(TMath::Abs(ancPDG)==11)
    {//e
      mcIndex = 1;
    }
    else if(ancPDG==111)
    {//Pi0
      mcIndex = 2;
      if(fMultiCutAnaSim)
      {
        for(Int_t ipt=0; ipt<fNPtCuts; ipt++)
        {
          for(Int_t icell=0; icell<fNCellNCuts; icell++)
          {
            for(Int_t iasym=0; iasym<fNAsymCuts; iasym++)
            {
              Int_t index = ((ipt*fNCellNCuts)+icell)*fNAsymCuts + iasym;
              if(pt1    >  fPtCuts[ipt]      && pt2    >  fPtCuts[ipt]        &&
                 asym   <  fAsymCuts[iasym]                                   &&
                 ncell1 >= fCellNCuts[icell] && ncell2 >= fCellNCuts[icell])
              {
                fhMCPi0MassPtRec [index]->Fill(pt,mass);
                fhMCPi0MassPtTrue[index]->Fill(ancMomentum.Pt(),mass);
                if(mass < 0.17 && mass > 0.1) fhMCPi0PtTruePtRec[index]->Fill(ancMomentum.Pt(),pt);
              }//pass the different cuts
            }// pid bit cut loop
          }// icell loop
        }// pt cut loop
      }//Multi cut ana sim
      else
      {
        fhMCPi0MassPtTrue[0]->Fill(ancMomentum.Pt(),mass);
        
        if(mass < 0.17 && mass > 0.1)
        {
          fhMCPi0PtTruePtRec[0]->Fill(ancMomentum.Pt(),pt);
          
          //Int_t uniqueId = -1;
          if(GetReader()->ReadStack())
          {
            TParticle* ancestor = GetMCStack()->Particle(ancLabel);
            momindex  = ancestor->GetFirstMother();
            if(momindex < 0) return;
            TParticle* mother = GetMCStack()->Particle(momindex);
            mompdg    = TMath::Abs(mother->GetPdgCode());
            momstatus = mother->GetStatusCode();
            prodR = mother->R();
            //uniqueId = mother->GetUniqueID();
          }
          else
          {
            TClonesArray * mcparticles = GetReader()->GetAODMCParticles();
            AliAODMCParticle* ancestor = (AliAODMCParticle *) mcparticles->At(ancLabel);
            momindex  = ancestor->GetMother();
            if(momindex < 0) return;
            AliAODMCParticle* mother = (AliAODMCParticle *) mcparticles->At(momindex);
            mompdg    = TMath::Abs(mother->GetPdgCode());
            momstatus = mother->GetStatus();
            prodR = TMath::Sqrt(mother->Xv()*mother->Xv()+mother->Yv()*mother->Yv());
            //uniqueId = mother->GetUniqueID();
          }
          
          //            printf("Reco Pi0: pt %2.2f Prod vertex %3.3f, origin pdg %d, origin status %d, origin UI %d\n",
          //                   pt,prodR,mompdg,momstatus,uniqueId);
          
          fhMCPi0ProdVertex->Fill(pt,prodR);
          
          if     (momstatus  == 21) fhMCPi0PtOrigin->Fill(pt,0.5);//parton
          else if(mompdg     < 22 ) fhMCPi0PtOrigin->Fill(pt,1.5);//quark
          else if(mompdg     > 2100  && mompdg   < 2210) fhMCPi0PtOrigin->Fill(pt,2.5);// resonances
          else if(mompdg    == 221) fhMCPi0PtOrigin->Fill(pt,8.5);//eta
          else if(mompdg    == 331) fhMCPi0PtOrigin->Fill(pt,9.5);//eta prime
          else if(mompdg    == 213) fhMCPi0PtOrigin->Fill(pt,4.5);//rho
          else if(mompdg    == 223) fhMCPi0PtOrigin->Fill(pt,5.5);//omega
          else if(mompdg    >= 310   && mompdg    <= 323) fhMCPi0PtOrigin->Fill(pt,6.5);//k0S, k+-,k*
          else if(mompdg    == 130) fhMCPi0PtOrigin->Fill(pt,6.5);//k0L
          else if(momstatus == 11 || momstatus  == 12 ) fhMCPi0PtOrigin->Fill(pt,3.5);//resonances
          else                      fhMCPi0PtOrigin->Fill(pt,7.5);//other?
          
          
        }//pi0 mass region
      }
    }
    else if(ancPDG==221)
    {//Eta
      mcIndex = 3;
      if(fMultiCutAnaSim)
      {
        for(Int_t ipt=0; ipt<fNPtCuts; ipt++)
        {
          for(Int_t icell=0; icell<fNCellNCuts; icell++)
          {
            for(Int_t iasym=0; iasym<fNAsymCuts; iasym++)
            {
              Int_t index = ((ipt*fNCellNCuts)+icell)*fNAsymCuts + iasym;
              if(pt1    >  fPtCuts[ipt]      && pt2    >  fPtCuts[ipt]        &&
                 asym   <  fAsymCuts[iasym]                                   &&
                 ncell1 >= fCellNCuts[icell] && ncell2 >= fCellNCuts[icell])
              {
                fhMCEtaMassPtRec [index]->Fill(pt,mass);
                fhMCEtaMassPtTrue[index]->Fill(ancMomentum.Pt(),mass);
                if(mass < 0.65 && mass > 0.45) fhMCEtaPtTruePtRec[index]->Fill(ancMomentum.Pt(),pt);
              }//pass the different cuts
            }// pid bit cut loop
          }// icell loop
        }// pt cut loop
      } //Multi cut ana sim
      else
      {
        fhMCEtaMassPtTrue[0]->Fill(ancMomentum.Pt(),mass);
        if(mass < 0.65 && mass > 0.45) fhMCEtaPtTruePtRec[0]->Fill(ancMomentum.Pt(),pt);
        
        if(GetReader()->ReadStack())
        {
          TParticle* ancestor = GetMCStack()->Particle(ancLabel);
          momindex  = ancestor->GetFirstMother();
          if(momindex < 0) return;
          TParticle* mother = GetMCStack()->Particle(momindex);
          mompdg    = TMath::Abs(mother->GetPdgCode());
          momstatus = mother->GetStatusCode();
          prodR = mother->R();
        }
        else
        {
          TClonesArray * mcparticles = GetReader()->GetAODMCParticles();
          AliAODMCParticle* ancestor = (AliAODMCParticle *) mcparticles->At(ancLabel);
          momindex  = ancestor->GetMother();
          if(momindex < 0) return;
          AliAODMCParticle* mother = (AliAODMCParticle *) mcparticles->At(momindex);
          mompdg    = TMath::Abs(mother->GetPdgCode());
          momstatus = mother->GetStatus();
          prodR = TMath::Sqrt(mother->Xv()*mother->Xv()+mother->Yv()*mother->Yv());
        }
        
        fhMCEtaProdVertex->Fill(pt,prodR);
        
        if     (momstatus == 21 ) fhMCEtaPtOrigin->Fill(pt,0.5);//parton
        else if(mompdg    < 22  ) fhMCEtaPtOrigin->Fill(pt,1.5);//quark
        else if(mompdg    > 2100  && mompdg   < 2210) fhMCEtaPtOrigin->Fill(pt,2.5);//qq resonances
        else if(mompdg    == 331) fhMCEtaPtOrigin->Fill(pt,5.5);//eta prime
        else if(momstatus == 11 || momstatus  == 12 ) fhMCEtaPtOrigin->Fill(pt,3.5);//resonances
        else fhMCEtaPtOrigin->Fill(pt,4.5);//stable, conversions?
        //printf("Other Meson pdg %d, Mother %s, pdg %d, status %d\n",pdg, TDatabasePDG::Instance()->GetParticle(mompdg)->GetName(),mompdg, momstatus );
        
      }// eta mass region
    }
    else if(ancPDG==-2212){//AProton
      mcIndex = 4;
    }
    else if(ancPDG==-2112){//ANeutron
      mcIndex = 5;
    }
    else if(TMath::Abs(ancPDG)==13){//muons
      mcIndex = 6;
    }
    else if (TMath::Abs(ancPDG) > 100 && ancLabel > 7)
    {
      if(ancStatus==1)
      {//Stable particles, converted? not decayed resonances
        mcIndex = 6;
      }
      else
      {//resonances and other decays, more hadron conversions?
        mcIndex = 7;
      }
    }
    else
    {//Partons, colliding protons, strings, intermediate corrections
      if(ancStatus==11 || ancStatus==12)
      {//String fragmentation
        mcIndex = 8;
      }
      else if (ancStatus==21){
        if(ancLabel < 2)
        {//Colliding protons
          mcIndex = 11;
        }//colliding protons
        else if(ancLabel < 6)
        {//partonic initial states interactions
          mcIndex = 9;
        }
        else if(ancLabel < 8)
        {//Final state partons radiations?
          mcIndex = 10;
        }
        // else {
        //   printf("AliAnaPi0::FillMCVersusRecDataHistograms() - Check ** Common ancestor label %d, pdg %d, name %s, status %d; \n",
        //          ancLabel,ancPDG,TDatabasePDG::Instance()->GetParticle(ancPDG)->GetName(),ancStatus);
        // }
      }//status 21
      //else {
      //  printf("AliAnaPi0::FillMCVersusRecDataHistograms() - Check *** Common ancestor label %d, pdg %d, name %s, status %d; \n",
      //         ancLabel,ancPDG,TDatabasePDG::Instance()->GetParticle(ancPDG)->GetName(),ancStatus);
      // }
    }////Partons, colliding protons, strings, intermediate corrections
  }//ancLabel > -1
  else { //ancLabel <= -1
    //printf("Not related at all label = %d\n",ancLabel);
    mcIndex = 12;
  }
  
  if(mcIndex >=0 && mcIndex < 13)
  {
    fhMCOrgMass[mcIndex]->Fill(pt,mass);
    fhMCOrgAsym[mcIndex]->Fill(pt,asym);
    fhMCOrgDeltaEta[mcIndex]->Fill(pt,deta);
    fhMCOrgDeltaPhi[mcIndex]->Fill(pt,dphi);
  }
  
}

//__________________________________________
void AliAnaPi0::MakeAnalysisFillHistograms()
{
  //Process one event and extract photons from AOD branch
  // filled with AliAnaPhoton and fill histos with invariant mass
  
  //In case of simulated data, fill acceptance histograms
  if(IsDataMC())FillAcceptanceHistograms();
  
  //if (GetReader()->GetEventNumber()%10000 == 0) 
  // printf("--- Event %d ---\n",GetReader()->GetEventNumber());
  
  if(!GetInputAODBranch())
  {
    printf("AliAnaPi0::MakeAnalysisFillHistograms() - No input aod photons in AOD with name branch < %s >, STOP \n",GetInputAODName().Data());
    abort();
  }
  
  //Init some variables
  Int_t   nPhot    = GetInputAODBranch()->GetEntriesFast() ;
  
  if(GetDebug() > 1) 
    printf("AliAnaPi0::MakeAnalysisFillHistograms() - Photon entries %d\n", nPhot);
  
  //If less than photon 2 entries in the list, skip this event
  if(nPhot < 2 )
  {
    if(GetDebug() > 2)
      printf("AliAnaPi0::MakeAnalysisFillHistograms() - nPhotons %d, cent bin %d continue to next event\n",nPhot, GetEventCentrality());
    
    if(GetNCentrBin() > 1) fhCentralityNoPair->Fill(GetEventCentrality() * GetNCentrBin() / GetReader()->GetCentralityOpt());
    
    return ;
  }
  
  Int_t ncentr = GetNCentrBin();
  if(GetNCentrBin()==0) ncentr = 1;
  
  //Init variables
  Int_t module1         = -1;
  Int_t module2         = -1;
  Double_t vert[]       = {0.0, 0.0, 0.0} ; //vertex 
  Int_t evtIndex1       = 0 ; 
  Int_t currentEvtIndex = -1; 
  Int_t curCentrBin     = GetEventCentralityBin();
  //Int_t curVzBin        = GetEventVzBin();
  //Int_t curRPBin        = GetEventRPBin();
  Int_t eventbin        = GetEventMixBin();
  
  if(eventbin > GetNCentrBin()*GetNZvertBin()*GetNRPBin())
  {
     printf("AliAnaPi0::MakeAnalysisFillHistograms() - Mix Bin not exepcted: cen bin %d, z bin %d, rp bin %d, total bin %d, Event Centrality %d, z vertex %2.3f, Reaction Plane %2.3f\n",GetEventCentralityBin(),GetEventVzBin(), GetEventRPBin(),eventbin,GetEventCentrality(),vert[2],GetEventPlaneAngle());
    return;
  }
    
  //Get shower shape information of clusters
  TObjArray *clusters = 0;
  if     (fCalorimeter=="EMCAL") clusters = GetEMCALClusters();
  else if(fCalorimeter=="PHOS" ) clusters = GetPHOSClusters() ;
  
  //---------------------------------
  //First loop on photons/clusters
  //---------------------------------
  for(Int_t i1=0; i1<nPhot-1; i1++)
  {
    AliAODPWG4Particle * p1 = (AliAODPWG4Particle*) (GetInputAODBranch()->At(i1)) ;
    //printf("AliAnaPi0::MakeAnalysisFillHistograms() : cluster1 id %d\n",p1->GetCaloLabel(0));
    
    // get the event index in the mixed buffer where the photon comes from 
    // in case of mixing with analysis frame, not own mixing
    evtIndex1 = GetEventIndex(p1, vert) ; 
    //printf("charge = %d\n", track->Charge());
    if ( evtIndex1 == -1 )
      return ; 
    if ( evtIndex1 == -2 )
      continue ; 
    
    //printf("z vertex %f < %f\n",vert[2],GetZvertexCut());
    if(TMath::Abs(vert[2]) > GetZvertexCut()) continue ;   //vertex cut
    
    
    if (evtIndex1 != currentEvtIndex) 
    {
      //Fill event bin info
      if(DoOwnMix()) fhEventBin->Fill(eventbin) ;
      if(GetNCentrBin() > 1) 
      {
        fhCentrality->Fill(curCentrBin);
        if(GetNRPBin() > 1 && GetEventPlane()) fhEventPlaneResolution->Fill(curCentrBin,TMath::Cos(2.*GetEventPlane()->GetQsubRes()));
      }
      currentEvtIndex = evtIndex1 ; 
    }
    
    //printf("AliAnaPi0::MakeAnalysisFillHistograms(): Photon 1 Evt %d  Vertex : %f,%f,%f\n",evtIndex1, GetVertex(evtIndex1)[0] ,GetVertex(evtIndex1)[1],GetVertex(evtIndex1)[2]);
    
    //Get the momentum of this cluster
    TLorentzVector photon1(p1->Px(),p1->Py(),p1->Pz(),p1->E());
    
    //Get (Super)Module number of this cluster
    module1 = GetModuleNumber(p1);
    
    //------------------------------------------
    // Recover original cluster
    Int_t iclus1 = -1 ;
    AliVCluster * cluster1 = FindCluster(clusters,p1->GetCaloLabel(0),iclus1);
    
    //---------------------------------
    //Second loop on photons/clusters
    //---------------------------------
    for(Int_t i2=i1+1; i2<nPhot; i2++)
    {
      AliAODPWG4Particle * p2 = (AliAODPWG4Particle*) (GetInputAODBranch()->At(i2)) ;
      
      //In case of mixing frame, check we are not in the same event as the first cluster
      Int_t evtIndex2 = GetEventIndex(p2, vert) ; 
      if ( evtIndex2 == -1 )
        return ; 
      if ( evtIndex2 == -2 )
        continue ;    
      if (GetMixedEvent() && (evtIndex1 == evtIndex2))
        continue ;
      
      //------------------------------------------
      // Recover original cluster
      AliVCluster * cluster2 = FindCluster(clusters,p2->GetCaloLabel(0),iclus1); // start new loop from iclus1 to gain some time
      
      Float_t tof1  = -1;
      Float_t l01   = -1;
      Int_t ncell1  = 0;
      if(cluster1)
      {
        tof1   = cluster1->GetTOF()*1e9;
        l01    = cluster1->GetM02();
        ncell1 = cluster1->GetNCells();
        //printf("cluster1: E %2.2f (%2.2f), l0 %2.2f, tof %2.2f\n",cluster1->E(),p1->E(),l01,tof1);
      }
      //else printf("cluster1 not available: calo label %d / %d, cluster ID %d\n",
      //            p1->GetCaloLabel(0),(GetReader()->GetInputEvent())->GetNumberOfCaloClusters()-1,cluster1->GetID());
      
      Float_t tof2  = -1;
      Float_t l02   = -1;
      Int_t ncell2  = 0;
      if(cluster2)
      {
        tof2   = cluster2->GetTOF()*1e9;
        l02    = cluster2->GetM02();
        ncell2 = cluster2->GetNCells();
        //printf("cluster2: E %2.2f (%2.2f), l0 %2.2f, tof %2.2f\n",cluster2->E(),p2->E(),l02,tof2);
      }
      //else printf("cluster2 not available: calo label %d / %d, cluster ID %d\n",
      //            p2->GetCaloLabel(0),(GetReader()->GetInputEvent())->GetNumberOfCaloClusters()-1,cluster2->GetID());
      
      if(cluster1 && cluster2)
      {
        Double_t t12diff = tof1-tof2;
        if(TMath::Abs(t12diff) > GetPairTimeCut()) continue;
      }
      //------------------------------------------
      
      //printf("AliAnaPi0::MakeAnalysisFillHistograms(): Photon 2 Evt %d  Vertex : %f,%f,%f\n",evtIndex2, GetVertex(evtIndex2)[0] ,GetVertex(evtIndex2)[1],GetVertex(evtIndex2)[2]);
      
      //Get the momentum of this cluster
      TLorentzVector photon2(p2->Px(),p2->Py(),p2->Pz(),p2->E());
      //Get module number
      module2       = GetModuleNumber(p2);
      
      //---------------------------------
      // Get pair kinematics
      //---------------------------------
      Double_t m    = (photon1 + photon2).M() ;
      Double_t pt   = (photon1 + photon2).Pt();
      Double_t deta = photon1.Eta() - photon2.Eta();
      Double_t dphi = photon1.Phi() - photon2.Phi();
      Double_t a    = TMath::Abs(p1->E()-p2->E())/(p1->E()+p2->E()) ;
      
      if(GetDebug() > 2)
        printf(" E: photon1 %f, photon2 %f; Pair: pT %f, mass %f, a %f\n", p1->E(), p2->E(), (photon1 + photon2).E(),m,a);
      
      //--------------------------------
      // Opening angle selection
      //--------------------------------
      //Check if opening angle is too large or too small compared to what is expected	
      Double_t angle   = photon1.Angle(photon2.Vect());
      if(fUseAngleEDepCut && !GetNeutralMesonSelection()->IsAngleInWindow((photon1+photon2).E(),angle+0.05))
      {
        if(GetDebug() > 2)
          printf("AliAnaPi0::MakeAnalysisFillHistograms() -Real pair angle %f not in E %f window\n",angle, (photon1+photon2).E());
        continue;
      }
      
      if(fUseAngleCut && (angle < fAngleCut || angle > fAngleMaxCut))
      {
        if(GetDebug() > 2)
          printf("AliAnaPi0::MakeAnalysisFillHistograms() - Real pair cut %f < angle %f < cut %f\n",fAngleCut, angle, fAngleMaxCut);
        continue;
      }
      
      //-------------------------------------------------------------------------------------------------
      //Fill module dependent histograms, put a cut on assymmetry on the first available cut in the array
      //-------------------------------------------------------------------------------------------------
      if(a < fAsymCuts[0] && fFillSMCombinations)
      {
        if(module1==module2 && module1 >=0 && module1<fNModules)
          fhReMod[module1]->Fill(pt,m) ;
        
        if(fCalorimeter=="EMCAL")
        {
          // Same sector
          Int_t j=0;
          for(Int_t i = 0; i < fNModules/2; i++)
          {
            j=2*i;
            if((module1==j && module2==j+1) || (module1==j+1 && module2==j)) fhReSameSectorEMCALMod[i]->Fill(pt,m) ;
          }
          
          // Same side
          for(Int_t i = 0; i < fNModules-2; i++){
            if((module1==i && module2==i+2) || (module1==i+2 && module2==i)) fhReSameSideEMCALMod[i]->Fill(pt,m); 
          }
        }//EMCAL
        else {//PHOS
          if((module1==0 && module2==1) || (module1==1 && module2==0)) fhReDiffPHOSMod[0]->Fill(pt,m) ; 
          if((module1==0 && module2==2) || (module1==2 && module2==0)) fhReDiffPHOSMod[1]->Fill(pt,m) ; 
          if((module1==1 && module2==2) || (module1==2 && module2==1)) fhReDiffPHOSMod[2]->Fill(pt,m) ;
        }//PHOS
      }
      
      //In case we want only pairs in same (super) module, check their origin.
      Bool_t ok = kTRUE;
      if(fSameSM && module1!=module2) ok=kFALSE;
      if(ok)
      {
        //Check if one of the clusters comes from a conversion 
        if(fCheckConversion)
        {
          if     (p1->IsTagged() && p2->IsTagged()) fhReConv2->Fill(pt,m);
          else if(p1->IsTagged() || p2->IsTagged()) fhReConv ->Fill(pt,m);
        }
        
        // Fill shower shape cut histograms
        if(fFillSSCombinations)
        {
          if     ( l01 > 0.01 && l01 < 0.4  && 
                   l02 > 0.01 && l02 < 0.4 )               fhReSS[0]->Fill(pt,m); // Tight
          else if( l01 > 0.4  && l02 > 0.4 )               fhReSS[1]->Fill(pt,m); // Loose
          else if( l01 > 0.01 && l01 < 0.4  && l02 > 0.4 ) fhReSS[2]->Fill(pt,m); // Both
          else if( l02 > 0.01 && l02 < 0.4  && l01 > 0.4 ) fhReSS[2]->Fill(pt,m); // Both
        }
        
        //Fill histograms for different bad channel distance, centrality, assymmetry cut and pid bit
        for(Int_t ipid=0; ipid<fNPIDBits; ipid++)
        {
          if((p1->IsPIDOK(fPIDBits[ipid],AliCaloPID::kPhoton)) && (p2->IsPIDOK(fPIDBits[ipid],AliCaloPID::kPhoton)))
          {
            for(Int_t iasym=0; iasym < fNAsymCuts; iasym++)
            {
              if(a < fAsymCuts[iasym])
              {
                Int_t index = ((curCentrBin*fNPIDBits)+ipid)*fNAsymCuts + iasym;
                //printf("index %d :(cen %d * nPID %d + ipid %d)*nasym %d + iasym %d - max index %d\n",index,curCentrBin,fNPIDBits,ipid,fNAsymCuts,iasym, curCentrBin*fNPIDBits*fNAsymCuts);
               
                if(index < 0 || index >= ncentr*fNPIDBits*fNAsymCuts) continue ;
                
                fhRe1     [index]->Fill(pt,m);
                if(fMakeInvPtPlots)fhReInvPt1[index]->Fill(pt,m,1./pt) ;
                if(fFillBadDistHisto)
                {
                  if(p1->DistToBad()>0 && p2->DistToBad()>0)
                  {
                    fhRe2     [index]->Fill(pt,m) ;
                    if(fMakeInvPtPlots)fhReInvPt2[index]->Fill(pt,m,1./pt) ;
                    if(p1->DistToBad()>1 && p2->DistToBad()>1)
                    {
                      fhRe3     [index]->Fill(pt,m) ;
                      if(fMakeInvPtPlots)fhReInvPt3[index]->Fill(pt,m,1./pt) ;
                    }// bad 3
                  }// bad2
                }// Fill bad dist histos
              }//assymetry cut
            }// asymmetry cut loop
          }// bad 1
        }// pid bit loop
        
        //Fill histograms with opening angle
        if(fFillAngleHisto)
        {
          fhRealOpeningAngle   ->Fill(pt,angle);
          fhRealCosOpeningAngle->Fill(pt,TMath::Cos(angle));
        }
        
        //Fill histograms with pair assymmetry
        if(fFillAsymmetryHisto)
        {
          fhRePtAsym->Fill(pt,a);
          if(m > 0.10 && m < 0.17) fhRePtAsymPi0->Fill(pt,a);
          if(m > 0.45 && m < 0.65) fhRePtAsymEta->Fill(pt,a);
        }
        
        //---------
        // MC data
        //---------
        //Do some MC checks on the origin of the pair, is there any common ancestor and if there is one, who?
        if(IsDataMC())
        {
          if(GetMCAnalysisUtils()->CheckTagBit(p1->GetTag(),AliMCAnalysisUtils::kMCConversion) && 
             GetMCAnalysisUtils()->CheckTagBit(p2->GetTag(),AliMCAnalysisUtils::kMCConversion))
          {
            fhReMCFromConversion->Fill(pt,m);
          }
          else if(!GetMCAnalysisUtils()->CheckTagBit(p1->GetTag(),AliMCAnalysisUtils::kMCConversion) && 
                  !GetMCAnalysisUtils()->CheckTagBit(p2->GetTag(),AliMCAnalysisUtils::kMCConversion))
          {
            fhReMCFromNotConversion->Fill(pt,m);
          }
          else
          {
            fhReMCFromMixConversion->Fill(pt,m);
          }
                  
          if(fFillOriginHisto)
            FillMCVersusRecDataHistograms(p1->GetLabel(), p2->GetLabel(),p1->Pt(), p2->Pt(),ncell1, ncell2, m, pt, a,deta, dphi);
        }
        
        //-----------------------
        //Multi cuts analysis
        //-----------------------
        if(fMultiCutAna)
        {
          //Histograms for different PID bits selection
          for(Int_t ipid=0; ipid<fNPIDBits; ipid++){
            
            if(p1->IsPIDOK(fPIDBits[ipid],AliCaloPID::kPhoton)    && 
               p2->IsPIDOK(fPIDBits[ipid],AliCaloPID::kPhoton))   fhRePIDBits[ipid]->Fill(pt,m) ;
            
            //printf("ipt %d, ipid%d, name %s\n",ipt, ipid, fhRePtPIDCuts[ipt*fNPIDBitsBits+ipid]->GetName());
          } // pid bit cut loop
          
          //Several pt,ncell and asymmetry cuts
          for(Int_t ipt=0; ipt<fNPtCuts; ipt++){          
            for(Int_t icell=0; icell<fNCellNCuts; icell++){
              for(Int_t iasym=0; iasym<fNAsymCuts; iasym++){
                Int_t index = ((ipt*fNCellNCuts)+icell)*fNAsymCuts + iasym;
                if(p1->E() >   fPtCuts[ipt]      && p2->E() > fPtCuts[ipt]        && 
                   a        <   fAsymCuts[iasym]                                    && 
                   ncell1   >=  fCellNCuts[icell] && ncell2   >= fCellNCuts[icell]){
                  fhRePtNCellAsymCuts[index]->Fill(pt,m) ;
                  //printf("ipt %d, icell%d, iasym %d, name %s\n",ipt, icell, iasym,  fhRePtNCellAsymCuts[((ipt*fNCellNCuts)+icell)*fNAsymCuts + iasym]->GetName());
                  if(fFillSMCombinations && module1==module2){
                    fhRePtNCellAsymCutsSM[module1][index]->Fill(pt,m) ;
                  }
                }
              }// pid bit cut loop
            }// icell loop
          }// pt cut loop
          if(GetHistogramRanges()->GetHistoTrackMultiplicityBins()){
            for(Int_t iasym = 0; iasym < fNAsymCuts; iasym++){
              if(a < fAsymCuts[iasym])fhRePtMult[iasym]->Fill(pt,GetTrackMultiplicity(),m) ;
            }
          }
        }// multiple cuts analysis
      }// ok if same sm
    }// second same event particle
  }// first cluster
  
  //-------------------------------------------------------------
  // Mixing
  //-------------------------------------------------------------
  if(DoOwnMix())
  {
    //Recover events in with same characteristics as the current event
    
    //Check that the bin exists, if not (bad determination of RP, centrality or vz bin) do nothing
    if(eventbin < 0) return ;
    
    TList * evMixList=fEventsList[eventbin] ;
    
    if(!evMixList)
    {
      printf("AliAnaPi0::MakeAnalysisFillHistograms() - Mix event list not available, bin %d \n",eventbin);
      return;
    }
    
    Int_t nMixed = evMixList->GetSize() ;
    for(Int_t ii=0; ii<nMixed; ii++)
    {  
      TClonesArray* ev2= (TClonesArray*) (evMixList->At(ii));
      Int_t nPhot2=ev2->GetEntriesFast() ;
      Double_t m = -999;
      if(GetDebug() > 1) 
        printf("AliAnaPi0::MakeAnalysisFillHistograms() - Mixed event %d photon entries %d, centrality bin %d\n", ii, nPhot2, GetEventCentralityBin());

      fhEventMixBin->Fill(eventbin) ;

      //---------------------------------
      //First loop on photons/clusters
      //---------------------------------      
      for(Int_t i1=0; i1<nPhot; i1++){
        AliAODPWG4Particle * p1 = (AliAODPWG4Particle*) (GetInputAODBranch()->At(i1)) ;
        if(fSameSM && GetModuleNumber(p1)!=module1) continue;
        
        //Get kinematics of cluster and (super) module of this cluster
        TLorentzVector photon1(p1->Px(),p1->Py(),p1->Pz(),p1->E());
        module1 = GetModuleNumber(p1);
        
        //---------------------------------
        //First loop on photons/clusters
        //---------------------------------        
        for(Int_t i2=0; i2<nPhot2; i2++){
          AliAODPWG4Particle * p2 = (AliAODPWG4Particle*) (ev2->At(i2)) ;
          
          //Get kinematics of second cluster and calculate those of the pair
          TLorentzVector photon2(p2->Px(),p2->Py(),p2->Pz(),p2->E());
          m           = (photon1+photon2).M() ; 
          Double_t pt = (photon1 + photon2).Pt();
          Double_t a  = TMath::Abs(p1->E()-p2->E())/(p1->E()+p2->E()) ;
          
          //Check if opening angle is too large or too small compared to what is expected
          Double_t angle   = photon1.Angle(photon2.Vect());
          if(fUseAngleEDepCut && !GetNeutralMesonSelection()->IsAngleInWindow((photon1+photon2).E(),angle+0.05)){ 
            if(GetDebug() > 2)
              printf("AliAnaPi0::MakeAnalysisFillHistograms() -Mix pair angle %f not in E %f window\n",angle, (photon1+photon2).E());
            continue;
          }
          if(fUseAngleCut && (angle < fAngleCut || angle > fAngleMaxCut)) {
            if(GetDebug() > 2)
              printf("AliAnaPi0::MakeAnalysisFillHistograms() -Mix pair angle %f < cut %f\n",angle,fAngleCut);
            continue; 
            
          } 
          
          if(GetDebug() > 2)
            printf("AliAnaPi0::MakeAnalysisFillHistograms() - Mixed Event: pT: photon1 %2.2f, photon2 %2.2f; Pair: pT %2.2f, mass %2.3f, a %f2.3\n",
                   p1->Pt(), p2->Pt(), pt,m,a);	
          
          //In case we want only pairs in same (super) module, check their origin.
          module2 = GetModuleNumber(p2);
          
          //-------------------------------------------------------------------------------------------------
          //Fill module dependent histograms, put a cut on assymmetry on the first available cut in the array
          //-------------------------------------------------------------------------------------------------          
          if(a < fAsymCuts[0] && fFillSMCombinations){
            if(module1==module2 && module1 >=0 && module1<fNModules)
              fhMiMod[module1]->Fill(pt,m) ;
            
            if(fCalorimeter=="EMCAL"){
              
              // Same sector
              Int_t j=0;
              for(Int_t i = 0; i < fNModules/2; i++){
                j=2*i;
                if((module1==j && module2==j+1) || (module1==j+1 && module2==j)) fhMiSameSectorEMCALMod[i]->Fill(pt,m) ;
              }
              
              // Same side
              for(Int_t i = 0; i < fNModules-2; i++){
                if((module1==i && module2==i+2) || (module1==i+2 && module2==i)) fhMiSameSideEMCALMod[i]->Fill(pt,m); 
              }
            }//EMCAL
            else {//PHOS
              if((module1==0 && module2==1) || (module1==1 && module2==0)) fhMiDiffPHOSMod[0]->Fill(pt,m) ; 
              if((module1==0 && module2==2) || (module1==2 && module2==0)) fhMiDiffPHOSMod[1]->Fill(pt,m) ; 
              if((module1==1 && module2==2) || (module1==2 && module2==1)) fhMiDiffPHOSMod[2]->Fill(pt,m) ;
            }//PHOS
            
            
          }
          
          Bool_t ok = kTRUE;
          if(fSameSM && module1!=module2) ok=kFALSE;
          if(ok){
            
            //Check if one of the clusters comes from a conversion 
            if(fCheckConversion){
              if     (p1->IsTagged() && p2->IsTagged()) fhMiConv2->Fill(pt,m);
              else if(p1->IsTagged() || p2->IsTagged()) fhMiConv ->Fill(pt,m);
            }
            //Fill histograms for different bad channel distance, centrality, assymmetry cut and pid bit
            for(Int_t ipid=0; ipid<fNPIDBits; ipid++){ 
              if((p1->IsPIDOK(ipid,AliCaloPID::kPhoton)) && (p2->IsPIDOK(ipid,AliCaloPID::kPhoton)))
              {
                for(Int_t iasym=0; iasym < fNAsymCuts; iasym++)
                {
                  if(a < fAsymCuts[iasym])
                  {
                    Int_t index = ((curCentrBin*fNPIDBits)+ipid)*fNAsymCuts + iasym;
                    
                    if(index < 0 || index >= ncentr*fNPIDBits*fNAsymCuts) continue ;

                    fhMi1     [index]->Fill(pt,m) ;
                    if(fMakeInvPtPlots)fhMiInvPt1[index]->Fill(pt,m,1./pt) ;
                    if(fFillBadDistHisto)
                    {
                      if(p1->DistToBad()>0 && p2->DistToBad()>0)
                      {
                        fhMi2     [index]->Fill(pt,m) ;
                        if(fMakeInvPtPlots)fhMiInvPt2[index]->Fill(pt,m,1./pt) ;
                        if(p1->DistToBad()>1 && p2->DistToBad()>1)
                        {
                          fhMi3     [index]->Fill(pt,m) ;
                          if(fMakeInvPtPlots)fhMiInvPt3[index]->Fill(pt,m,1./pt) ;
                        }
                      }
                    }// Fill bad dist histo
                  }//Asymmetry cut
                }// Asymmetry loop
              }//PID cut
            }//loop for histograms
            
            //-----------------------
            //Multi cuts analysis 
            //-----------------------            
            if(fMultiCutAna){
              //Several pt,ncell and asymmetry cuts
              
              for(Int_t ipt=0; ipt<fNPtCuts; ipt++){          
                for(Int_t icell=0; icell<fNCellNCuts; icell++){
                  for(Int_t iasym=0; iasym<fNAsymCuts; iasym++){
                    Int_t index = ((ipt*fNCellNCuts)+icell)*fNAsymCuts + iasym;
                    if(p1->Pt() >   fPtCuts[ipt]      && p2->Pt() > fPtCuts[ipt]        && 
                       a        <   fAsymCuts[iasym]                                    //&& 
                       //p1->GetBtag() >=  fCellNCuts[icell] && p2->GetBtag() >= fCellNCuts[icell] // trick, correct it.
                       ){
                      fhMiPtNCellAsymCuts[index]->Fill(pt,m) ;
                      //printf("ipt %d, icell%d, iasym %d, name %s\n",ipt, icell, iasym,  fhRePtNCellAsymCuts[((ipt*fNCellNCuts)+icell)*fNAsymCuts + iasym]->GetName());
                    }
                  }// pid bit cut loop
                }// icell loop
              }// pt cut loop
            } // Multi cut ana
            
            //Fill histograms with opening angle
            if(fFillAngleHisto)
            {
              fhMixedOpeningAngle   ->Fill(pt,angle);
              fhMixedCosOpeningAngle->Fill(pt,TMath::Cos(angle));
            }
            
          }//ok
        }// second cluster loop
      }//first cluster loop
    }//loop on mixed events
    
    //--------------------------------------------------------
    //Add the current event to the list of events for mixing
    //--------------------------------------------------------
    TClonesArray *currentEvent = new TClonesArray(*GetInputAODBranch());
    //Add current event to buffer and Remove redundant events 
    if(currentEvent->GetEntriesFast()>0){
      evMixList->AddFirst(currentEvent) ;
      currentEvent=0 ; //Now list of particles belongs to buffer and it will be deleted with buffer
      if(evMixList->GetSize() >= GetNMaxEvMix())
      {
        TClonesArray * tmp = (TClonesArray*) (evMixList->Last()) ;
        evMixList->RemoveLast() ;
        delete tmp ;
      }
    } 
    else{ //empty event
      delete currentEvent ;
      currentEvent=0 ; 
    }
  }// DoOwnMix
  
}	

//________________________________________________________________________
Int_t AliAnaPi0::GetEventIndex(AliAODPWG4Particle * part, Double_t * vert)  
{
  // retieves the event index and checks the vertex
  //    in the mixed buffer returns -2 if vertex NOK
  //    for normal events   returns 0 if vertex OK and -1 if vertex NOK
  
  Int_t evtIndex = -1 ; 
  if(GetReader()->GetDataType()!=AliCaloTrackReader::kMC){
    
    if (GetMixedEvent()){
      
      evtIndex = GetMixedEvent()->EventIndexForCaloCluster(part->GetCaloLabel(0)) ;
      GetVertex(vert,evtIndex); 
      
      if(TMath::Abs(vert[2])> GetZvertexCut())
        evtIndex = -2 ; //Event can not be used (vertex, centrality,... cuts not fulfilled)
    } else {// Single event
      
      GetVertex(vert);
      
      if(TMath::Abs(vert[2])> GetZvertexCut())
        evtIndex = -1 ; //Event can not be used (vertex, centrality,... cuts not fulfilled)
      else 
        evtIndex = 0 ;
    }
  }//No MC reader
  else {
    evtIndex = 0;
    vert[0] = 0. ; 
    vert[1] = 0. ; 
    vert[2] = 0. ; 
  }
  
  return evtIndex ; 
}

