/***************************************************************************
              adash@cern.ch - last modified on 03/04/2013
            prottay.das@cern.ch - last modified on 29/05/2021

// *** Configuration script for Phi-Meson analysis with 2013 PPb runs ***
// 
// A configuration script for RSN package needs to define the followings:
//
// (1) decay tree of each resonance to be studied, which is needed to select
//     true pairs and to assign the right mass to all candidate daughters
// (2) cuts at all levels: single daughters, tracks, events
// (3) output objects: histograms or trees
****************************************************************************/

Bool_t ConfigRhoKaon(  
			AliRsnMiniAnalysisTask *task, 
			Bool_t                 isMC, 
			Bool_t                 isPP,
			const char             *suffix,
			Float_t                nsigmapionTPC = 2.0,
			Float_t                nsigmakaonTPC  =2.0,
			Float_t                nsigmapionTOF = 3.0,
			Float_t                nsigmakaonTOF = 3.0,
			int                    ptpk         =0,
                        int                    pid          =1,
			Float_t                lmassrho     =0.5,
			Float_t                hmassrho     =0.9,
			Float_t                linvmass     =0.5,
			Float_t                hinvmass     =0.9,
			int                    invmassbins  =1,
			Bool_t                 enableMonitor = kTRUE,
			TString                optSyt="DefaultITSTPC2011"
			  )
{

 
  
  
  AliPID::EParticleType  type1   = AliPID::kPion;
  AliPID::EParticleType  type2   = AliPID::kKaon;
  // manage suffix
  if (strlen(suffix) > 0) suffix = Form("_%s", suffix);
  
  AliRsnCutSetDaughterParticle* cutSetPi;
  AliRsnCutSetDaughterParticle* cutSetK;
  AliRsnCutTrackQuality* trkQualityCut= new AliRsnCutTrackQuality("myQualityCut");
  if(optSyt.Contains("DefaultITSTPC2011"))
{ 
if (ptpk==0)
{
    trkQualityCut->SetDefaults2011(kTRUE,kTRUE);
}
/*
else if (ptpk==1)
{
    trkQualityCut->SetDefaults2011(kTRUE,kTRUE);
    trkQualityCut->SetPtRange(0.3,20);
}
else if (ptpk==2)
{
    trkQualityCut->SetDefaults2011(kTRUE,kTRUE);
    trkQualityCut->SetPtRange(0.5,20);
}
else if (ptpk==3)
{
    trkQualityCut->SetDefaults2011(kTRUE,kTRUE);
    trkQualityCut->SetPtRange(0.3,0.7);
}
*/
    Printf(Form("::::: SetCustomQualityCut:: using standard 2011 track quality cuts"));
 }
  else if(optSyt.Contains("DefaultTPCOnly")){
    trkQualityCut->SetDefaultsTPCOnly(kTRUE);
    Printf(Form("::::: SetCustomQualityCut:: using TPC-only track quality cuts"));
  }




if (pid==0)  //TPCTOF
{
  
  //*******************************************************************  
  cutSetPi=new AliRsnCutSetDaughterParticle(Form("cutPi%i_%2.1fsigma",AliRsnCutSetDaughterParticle::kTPCTOFpidphipp2015,nsigmapionTPC),trkQualityCut,AliRsnCutSetDaughterParticle::kTPCTOFpidphipp2015,AliPID::kPion,nsigmapionTPC,nsigmapionTOF);
  cutSetK=new AliRsnCutSetDaughterParticle(Form("cutK%i_%2.1fsigma",AliRsnCutSetDaughterParticle::kTPCTOFpidphipp2015,nsigmakaonTPC),trkQualityCut,AliRsnCutSetDaughterParticle::kTPCTOFpidphipp2015,AliPID::kKaon,nsigmakaonTPC,nsigmakaonTOF);
  //***********************************************************************
}

else if (pid==1) //TPC momentum independent
{
  
  //**************************************************************************
  cutSetPi=new AliRsnCutSetDaughterParticle(Form("cutPi%i_%2.1fsigma",AliRsnCutSetDaughterParticle::kFastTPCpidNsigma,nsigmapionTPC),trkQualityCut,AliRsnCutSetDaughterParticle::kFastTPCpidNsigma,AliPID::kPion,nsigmapionTPC,-1.0);
  
   cutSetK=new AliRsnCutSetDaughterParticle(Form("cutK%i_%2.1fsigma",AliRsnCutSetDaughterParticle::kFastTPCpidNsigma,nsigmakaonTPC),trkQualityCut,AliRsnCutSetDaughterParticle::kFastTPCpidNsigma,AliPID::kKaon,nsigmakaonTPC,-1.0);
  //*************************************************************************

}

else if (pid==2) //TPC momentum depended
{

  //***************************************************************************
cutSetPi=new AliRsnCutSetDaughterParticle(Form("cutPi%i_%2.1fsigma",AliRsnCutSetDaughterParticle::kTPCPidPtDep,nsigmapionTPC),trkQualityCut,AliRsnCutSetDaughterParticle::kTPCPidPtDep,AliPID::kPion,nsigmapionTPC,-1.0);
  
cutSetK=new AliRsnCutSetDaughterParticle(Form("cutK%i_%2.1fsigma",AliRsnCutSetDaughterParticle::kTPCPidPtDep,nsigmakaonTPC),trkQualityCut,AliRsnCutSetDaughterParticle::kTPCPidPtDep,AliPID::kKaon,nsigmakaonTPC,-1.0);
  //*************************************************************************
}


else if (pid==3) //TPC strongly momentum depended
{

  //***************************************************************************
cutSetPi=new AliRsnCutSetDaughterParticle(Form("cutPi%i_%2.1fsigma",AliRsnCutSetDaughterParticle::kTPCpidphipp2015,nsigmapionTPC),trkQualityCut,AliRsnCutSetDaughterParticle::kTPCpidphipp2015,AliPID::kPion,nsigmapionTPC,-1.0);
  
cutSetK=new AliRsnCutSetDaughterParticle(Form("cutK%i_%2.1fsigma",AliRsnCutSetDaughterParticle::kTPCpidphipp2015,nsigmakaonTPC),trkQualityCut,AliRsnCutSetDaughterParticle::kTPCpidphipp2015,AliPID::kKaon,nsigmakaonTPC,-1.0);
  //*************************************************************************
}



else if (pid==4) //TOF momentum independent
{

  //***************************************************************************
 
cutSetPi=new AliRsnCutSetDaughterParticle(Form("cutPi%i_%2.1fsigma",AliRsnCutSetDaughterParticle::kFastTOFpidNsigma,nsigmapionTOF),trkQualityCut,AliRsnCutSetDaughterParticle::kFastTOFpidNsigma,AliPID::kPion,-1.0,nsigmapionTOF);
  
  cutSetK=new AliRsnCutSetDaughterParticle(Form("cutK%i_%2.1fsigma",AliRsnCutSetDaughterParticle::kFastTOFpidNsigma,nsigmakaonTOF),trkQualityCut,AliRsnCutSetDaughterParticle::kFastTOFpidNsigma,AliPID::kKaon,-1.0,nsigmakaonTOF);
  //*************************************************************************
}



    Int_t iCutPi = task->AddTrackCuts(cutSetPi);
    Int_t iCutK = task->AddTrackCuts(cutSetK);
    
    if(enableMonitor){
      Printf("======== Monitoring cut AliRsnCutSetDaughterParticle enabled");
      gROOT->LoadMacro("$ALICE_PHYSICS/PWGLF/RESONANCES/macros/mini/AddMonitorOutput.C");
    AddMonitorOutput(isMC, cutSetPi->GetMonitorOutput());
    AddMonitorOutput(isMC, cutSetK->GetMonitorOutput());

    }
    
  AliRsnCutMiniPair* cutYRes=new AliRsnCutMiniPair("cutRapidityResonance",AliRsnCutMiniPair::kRapidityRange);
  cutYRes->SetRangeD(-0.8,0.8);
  
  AliRsnMiniResonanceFinder* Rhofinder= new AliRsnMiniResonanceFinder("rho");
  /*AliRsnMiniResonanceFinder* Kstarfinderp= new AliRsnMiniResonanceFinder("kstar");
  AliRsnMiniResonanceFinder* Kstarfinderm= new AliRsnMiniResonanceFinder("antikstar");
 /////////////////////////////////////////////////////////////////////////////
 */
  AliRsnMiniResonanceFinder* Rhofalsefinder=new AliRsnMiniResonanceFinder("rhobkg");
  Int_t iCutrhofalse;
  AliRsnMiniResonanceFinder* Rhofalsefinderminus=new AliRsnMiniResonanceFinder("rhobkgminus");
  Int_t iCutrhofalseminus;
  ///////////////////////////////////////////////////////////////////////////


  Int_t iCutKstarp,iCutKstarm,iCutRho;
  
  /* AliRsnCutMiniPair* cutMassKstar=new AliRsnCutMiniPair("cutMassKstar",AliRsnCutMiniPair::kMassRange);
     cutMassKstar->SetRangeD(0.7,1.1);*/

  AliRsnCutMiniPair* cutMassRho=new AliRsnCutMiniPair("cutMassRho",AliRsnCutMiniPair::kMassRange);
  //cutMassRho->SetRangeD(0.320,1.220);
  cutMassRho->SetRangeD(lmassrho,hmassrho);

  /*
  AliRsnCutSet* cutsKstar=new AliRsnCutSet("pairCutsKstar",AliRsnTarget::kMother);
  cutsKstar->AddCut(cutMassKstar);
  cutsKstar->AddCut(cutYRes);
  cutsKstar->SetCutScheme(TString::Format("%s&%s",cutMassKstar->GetName(),cutYRes->GetName()).Data());
  */
  AliRsnCutSet* cutsRho=new AliRsnCutSet("pairCutsRho",AliRsnTarget::kMother);
  cutsRho->AddCut(cutMassRho);
  cutsRho->AddCut(cutYRes);
  cutsRho->SetCutScheme(TString::Format("%s&%s",cutMassRho->GetName(),cutYRes->GetName()).Data());
  


 //////////////////////////////////////////////////////////////////////////////
  AliRsnCutSet* cutsRhofalse=new AliRsnCutSet("likepairCutsRho",AliRsnTarget::kMother);
  cutsRhofalse->AddCut(cutMassRho);
  cutsRhofalse->AddCut(cutYRes);
  cutsRhofalse->SetCutScheme(TString::Format("%s&%s",cutMassRho->GetName(),cutYRes->GetName()).Data());
  //////////////////////////////////////////////////////////////////////////////









  /*
  Kstarfinderp->SetCutID(0,iCutK);
  Kstarfinderp->SetDaughter(0,AliRsnDaughter::kKaon);
  Kstarfinderp->SetCharge(0,'+');
  Kstarfinderp->SetCutID(1,iCutPi);
  Kstarfinderp->SetDaughter(1,AliRsnDaughter::kPion);
  Kstarfinderp->SetCharge(1,'-');
  //Kstarfinderp->SetResonanceMass(0.896);
  //Kstarfinderp->SetResonancePDG(313);
  Kstarfinderp->SetPairCuts(cutsKstar);
  iCutKstarp=task->AddResonanceFinder(Kstarfinderp);

  Kstarfinderm->SetCutID(0,iCutK);
  Kstarfinderm->SetDaughter(0,AliRsnDaughter::kKaon);
  Kstarfinderm->SetCharge(0,'-');
  Kstarfinderm->SetCutID(1,iCutPi);
  Kstarfinderm->SetDaughter(1,AliRsnDaughter::kPion);
  Kstarfinderm->SetCharge(1,'+');
  //Kstarfinderm->SetResonanceMass(0.896);
  //Kstarfinderm->SetResonancePDG(313);
  Kstarfinderm->SetPairCuts(cutsKstar);
  iCutKstarm=task->AddResonanceFinder(Kstarfinderm);
  */
  Rhofinder->SetCutID(0,iCutPi);
  Rhofinder->SetDaughter(0,AliRsnDaughter::kPion);
  Rhofinder->SetCharge(0,'+');
  Rhofinder->SetCutID(1,iCutPi);
  Rhofinder->SetDaughter(1,AliRsnDaughter::kPion);
  Rhofinder->SetCharge(1,'-');
  //Rhofinder->SetResonanceMass(0.775);
  //Rhofinder->SetResonancePDG(113);
  Rhofinder->SetPairCuts(cutsRho);
  iCutRho=task->AddResonanceFinder(Rhofinder);




////////////////////////////////////////////////////////////////////////////

  Rhofalsefinder->SetCutID(0,iCutPi);
  Rhofalsefinder->SetDaughter(0,AliRsnDaughter::kPion);
  Rhofalsefinder->SetCharge(0,'+');
  Rhofalsefinder->SetCutID(1,iCutPi);
  Rhofalsefinder->SetDaughter(1,AliRsnDaughter::kPion);
  Rhofalsefinder->SetCharge(1,'+');
  //Rhofinder->SetResonanceMass(0.775);
  //Rhofinder->SetResonancePDG(113);
  Rhofalsefinder->SetPairCuts(cutsRhofalse);
  iCutrhofalse=task->AddResonanceFinder(Rhofalsefinder);


  Rhofalsefinderminus->SetCutID(0,iCutPi);
  Rhofalsefinderminus->SetDaughter(0,AliRsnDaughter::kPion);
  Rhofalsefinderminus->SetCharge(0,'-');
  Rhofalsefinderminus->SetCutID(1,iCutPi);
  Rhofalsefinderminus->SetDaughter(1,AliRsnDaughter::kPion);
  Rhofalsefinderminus->SetCharge(1,'-');
  //Rhofinder->SetResonanceMass(0.775);
  //Rhofinder->SetResonancePDG(113);
  Rhofalsefinderminus->SetPairCuts(cutsRhofalse);
  iCutrhofalseminus=task->AddResonanceFinder(Rhofalsefinderminus);


///////////////////////////////////////////////////////////////////////////////











  
  
  // -- Values ------------------------------------------------------------------------------------
  /* invariant mass   */ Int_t imID   = task->CreateValue(AliRsnMiniValue::kInvMass,    kFALSE);
  /* IM resolution    */ Int_t resID  = task->CreateValue(AliRsnMiniValue::kInvMassRes, kTRUE);
  /* transv. momentum */ Int_t ptID   = task->CreateValue(AliRsnMiniValue::kPt,         kFALSE);
  /* centrality       */ Int_t centID = task->CreateValue(AliRsnMiniValue::kMult,       kFALSE);
  /* pseudorapidity   */ Int_t etaID  = task->CreateValue(AliRsnMiniValue::kEta,        kFALSE);
  /* rapidity         */ Int_t yID    = task->CreateValue(AliRsnMiniValue::kY,          kFALSE);

  // -- Create all needed outputs -----------------------------------------------------------------
  // use an array for more compact writing, which are different on mixing and charges
  // [0] = unlike
  // [1] = mixing
  // [2] = like ++
  // [3] = like --

  AliRsnCutMiniPair* cutY=new AliRsnCutMiniPair("cutRapidity", AliRsnCutMiniPair::kRapidityRange);
  cutY->SetRangeD(-0.5,0.5);
   
  AliRsnCutSet* cutsPairSame=new AliRsnCutSet("pairCutsSame",AliRsnTarget::kMother);
  cutsPairSame->AddCut(cutY);
  cutsPairSame->SetCutScheme(TString::Format("%s",cutY->GetName()).Data());
  
  AliRsnCutSet* cutsPairMix=new AliRsnCutSet("pairCutsMix", AliRsnTarget::kMother);
  cutsPairMix->AddCut(cutY);
  cutsPairMix->SetCutScheme(cutY->GetName());

  


  Bool_t  use     [6] = {1         ,      1,              1,              1,                1,                          1};
  Bool_t  useIM   [6] = {1         ,      1,              1,              1,                1,                          1};
  TString name    [6] = {"UnlikeP",  "UnlikeM",       "MixingP",      "MixingM",      " Mixingrholikeminus",   "Mixingrholike"};
  TString comp    [6] = {"PAIR"    ,   "PAIR",          "MIX",           "MIX",          "MIX",                       "MIX"};
  TString output  [6] = {"SPARSE"  ,  "SPARSE",        "SPARSE",      "SPARSE",         "SPARSE",                  "SPARSE"};
  Char_t  charge1 [6] = {  '0'     ,     '0',             '0'  ,          '0',            '0',                        '0'};
  Char_t  charge2 [6] = {  '+'     ,     '-',             '+',           '-',             '+',                        '-'};
  Int_t   cutIDPi [6] = {iCutRho   ,   iCutRho,         iCutRho,        iCutRho,       iCutrhofalseminus,      iCutrhofalse};
  Int_t   cutIDK  [6] = {iCutK     ,    iCutK,           iCutK,          iCutK,           iCutK,                     iCutK};
  Int_t   PDGCode [6] = {10323     ,    10323,           10323,          10323,           10323,                     10323};



  for (Int_t i = 0; i < 6; i++) {
    if (!use[i]) continue;
    AliRsnMiniOutput *out = task->CreateOutput(Form("KSTAR_%s%s", name[i].Data(), suffix), output[i].Data(), comp[i].Data());
    out->SetDaughter(0, AliRsnDaughter::kPhi);//rho
    out->SetDaughter(1, AliRsnDaughter::kKaon);
    out->SetCutID(0, cutIDPi[i]);
    out->SetCutID(1, cutIDK[i]);
    out->SetCharge(0, charge1[i]);
    out->SetCharge(1, charge2[i]);
    out->SetUseStoredMass(0);
    //out->SetMotherPDG(PDGCode[i]);
    //out->SetMotherMass(1.272);
    if(i<=0) out->SetPairCuts(cutsPairSame);
    else out->SetPairCuts(cutsPairMix);
    out->AddAxis(imID, invmassbins, linvmass, hinvmass);
    out->AddAxis(ptID, 100, 0.0, 10.0);
    out->AddAxis(centID, 100, 0.0, 100.0);
  }
  /*
  out=task->CreateOutput("kstar0","HIST","PAIR");
  out->SetMotherPDG(Kstarfinderp->GetResonancePDG());
  
  out->SetDaughter(0,Kstarfinderp->GetDaughter(0));
  out->SetCutID(0,Kstarfinderp->GetCutID(0));
  out->SetCharge(0,Kstarfinderp->GetCharge(0));
  
  out->SetDaughter(1,Kstarfinderp->GetDaughter(1));
  out->SetCutID(1,Kstarfinderp->GetCutID(1));
  out->SetCharge(1,Kstarfinderp->GetCharge(1));
  
  out->SetMotherMass(Kstarfinderp->GetResonanceMass());
  out->SetPairCuts(cutsKstar);
  out->AddAxis(imID,90,0.6,1.5);
  */

  /*
  out=task->CreateOutput("rho0","HIST","PAIR");
  out->SetMotherPDG(Rhofinder->GetResonancePDG());
  
  out->SetDaughter(0,Rhofinder->GetDaughter(0));
  out->SetCutID(0,Rhofinder->GetCutID(0));
  out->SetCharge(0,Rhofinder->GetCharge(0));
  
  out->SetDaughter(1,Rhofinder->GetDaughter(1));
  out->SetCutID(1,Rhofinder->GetCutID(1));
  out->SetCharge(1,Rhofinder->GetCharge(1));
  
  out->SetMotherMass(Rhofinder->GetResonanceMass());
  out->SetPairCuts(cutsRho);
  out->AddAxis(imID,80,0.3,2.1);
  */
  
  /*
  out=task->CreateOutput("rholikebkg","HIST","PAIR");
  out->SetMotherPDG( Rhofalsefinder->GetResonancePDG());

  out->SetDaughter(0, Rhofalsefinder->GetDaughter(0));
  out->SetCutID(0, Rhofalsefinder->GetCutID(0));
  out->SetCharge(0, Rhofalsefinder->GetCharge(0));

  out->SetDaughter(1, Rhofalsefinder->GetDaughter(1));
  out->SetCutID(1, Rhofalsefinder->GetCutID(1));
  out->SetCharge(1, Rhofalsefinder->GetCharge(1));

  out->SetMotherMass( Rhofalsefinder->GetResonanceMass());
  out->SetPairCuts(cutsRhofalse);
  out->AddAxis(imID,80,0.3,1.1);
  */

  

  return kTRUE;
}
//=============================


void AddMonitorOutput_P(TString name,TObjArray *mon,TString opt,AliRsnLoopDaughter *loop){
  AliRsnValueDaughter* a=new AliRsnValueDaughter(Form("%s_mom",name.Data()),AliRsnValueDaughter::kP);
  a->SetBins(0.,10.0,0.05);
  AliRsnListOutput* o=new AliRsnListOutput(Form("out_%s",a->GetName()),AliRsnListOutput::kHistoDefault);
  o->AddValue(a);
  if (mon) mon->Add(o);
  if (loop) loop->AddOutput(o);
}


void AddMonitorOutput_Pt(TString name,TObjArray *mon,TString opt,AliRsnLoopDaughter *loop){
  AliRsnValueDaughter* a=new AliRsnValueDaughter(Form("%s_pt",name.Data()),AliRsnValueDaughter::kPt);
  a->SetBins(0.,10.0,0.05);
  AliRsnListOutput* o=new AliRsnListOutput(Form("out_%s",a->GetName()),AliRsnListOutput::kHistoDefault);
  o->AddValue(a);
  if (mon) mon->Add(o);
  if (loop) loop->AddOutput(o);
}

void AddMonitorOutput_Eta(TString name,TObjArray *mon,TString opt,AliRsnLoopDaughter *loop){
  AliRsnValueDaughter* a=new AliRsnValueDaughter(Form("%s_eta",name.Data()),AliRsnValueDaughter::kEta);
  a->SetBins(-2.,2.,0.01);
  AliRsnListOutput* o=new AliRsnListOutput(Form("out_%s",a->GetName()),AliRsnListOutput::kHistoDefault);
  o->AddValue(a);
  if (mon) mon->Add(o);
  if (loop) loop->AddOutput(o);
}

void AddMonitorOutput_DCAxy(TString name,TObjArray *mon,TString opt,AliRsnLoopDaughter *loop){
  AliRsnValueDaughter* a=new AliRsnValueDaughter(Form("%s_dcaxy",name.Data()),AliRsnValueDaughter::kDCAXY);
  a->SetBins(-0.5,0.5,0.001);
  AliRsnListOutput* o=new AliRsnListOutput(Form("out_%s",a->GetName()),AliRsnListOutput::kHistoDefault);
  o->AddValue(a);
  if (mon) mon->Add(o);
  if (loop) loop->AddOutput(o);
}

void AddMonitorOutput_DCAz(TString name,TObjArray *mon,TString opt,AliRsnLoopDaughter *loop){
  AliRsnValueDaughter* a=new AliRsnValueDaughter(Form("%s_dcaz",name.Data()),AliRsnValueDaughter::kDCAZ);
  a->SetBins(-2.5,2.5,0.005);
  AliRsnListOutput* o=new AliRsnListOutput(Form("out_%s",a->GetName()),AliRsnListOutput::kHistoDefault);
  o->AddValue(a);
  if (mon) mon->Add(o);
  if (loop) loop->AddOutput(o);
}

void AddMonitorOutput_TPCpi(TString name,TObjArray *mon,TString opt,AliRsnLoopDaughter *loop){
  AliRsnValueDaughter* a=new AliRsnValueDaughter(Form("%s_TPCpi",name.Data()),AliRsnValueDaughter::kTPCnsigmaPi);
  a->SetBins(-10.,10.,0.01);
  AliRsnListOutput* o=new AliRsnListOutput(Form("out_%s",a->GetName()),AliRsnListOutput::kHistoDefault);
  o->AddValue(a);
  if (mon) mon->Add(o);
  if (loop) loop->AddOutput(o);
}

void AddMonitorOutput_TPCK(TString name,TObjArray *mon,TString opt,AliRsnLoopDaughter *loop){
  AliRsnValueDaughter* a=new AliRsnValueDaughter(Form("%s_TPCK",name.Data()),AliRsnValueDaughter::kTPCnsigmaK);
  a->SetBins(-10.,10.,0.01);
  AliRsnListOutput* o=new AliRsnListOutput(Form("out_%s",a->GetName()),AliRsnListOutput::kHistoDefault);
  o->AddValue(a);
  if (mon) mon->Add(o);
  if (loop) loop->AddOutput(o);
}

void AddMonitorOutput_TPCp(TString name,TObjArray *mon,TString opt,AliRsnLoopDaughter *loop){
  AliRsnValueDaughter* a=new AliRsnValueDaughter(Form("%s_TPCp",name.Data()),AliRsnValueDaughter::kTPCnsigmaP);
  a->SetBins(-10.,10.,0.01);
  AliRsnListOutput* o=new AliRsnListOutput(Form("out_%s",a->GetName()),AliRsnListOutput::kHistoDefault);
  o->AddValue(a);
  if (mon) mon->Add(o);
  if (loop) loop->AddOutput(o);
}

void AddMonitorOutput_NclTPC(TString name,TObjArray *mon,TString opt,AliRsnLoopDaughter *loop){
  AliRsnValueDaughter* a=new AliRsnValueDaughter(Form("%s_NclTPC",name.Data()),AliRsnValueDaughter::kNTPCclusters);
  a->SetBins(-0.5,199.5,1);
  AliRsnListOutput* o=new AliRsnListOutput(Form("out_%s",a->GetName()),AliRsnListOutput::kHistoDefault);
  o->AddValue(a);
  if (mon) mon->Add(o);
  if (loop) loop->AddOutput(o);
}

void AddMonitorOutput_chi2TPC(TString name,TObjArray *mon,TString opt,AliRsnLoopDaughter *loop){
  AliRsnValueDaughter* a=new AliRsnValueDaughter(Form("%s_chi2TPC",name.Data()),AliRsnValueDaughter::kTPCchi2);
  a->SetBins(0.0,6,.1);
  AliRsnListOutput* o=new AliRsnListOutput(Form("out_%s",a->GetName()),AliRsnListOutput::kHistoDefault);
  o->AddValue(a);
  if (mon) mon->Add(o);
  if (loop) loop->AddOutput(o);
}

void AddMonitorOutput_V0NPt(TString name,TObjArray *mon,TString opt,AliRsnLoopDaughter *loop){
  AliRsnValueDaughter* a=new AliRsnValueDaughter(Form("%s_v0npt",name.Data()),AliRsnValueDaughter::kV0NPt);
  a->SetBins(0.,10.0,0.05);
  AliRsnListOutput* o=new AliRsnListOutput(Form("out_%s",a->GetName()),AliRsnListOutput::kHistoDefault);
  o->AddValue(a);
  if (mon) mon->Add(o);
  if (loop) loop->AddOutput(o);
}

void AddMonitorOutput_V0PPt(TString name,TObjArray *mon,TString opt,AliRsnLoopDaughter *loop){
  AliRsnValueDaughter* a=new AliRsnValueDaughter(Form("%s_v0ppt",name.Data()),AliRsnValueDaughter::kV0PPt);
  a->SetBins(0.,10.0,0.05);
  AliRsnListOutput* o=new AliRsnListOutput(Form("out_%s",a->GetName()),AliRsnListOutput::kHistoDefault);
  o->AddValue(a);
  if (mon) mon->Add(o);
  if (loop) loop->AddOutput(o);
}

void AddMonitorOutput_V0Mass(TString name,TObjArray *mon,TString opt,AliRsnLoopDaughter *loop){
  AliRsnValueDaughter* a=new AliRsnValueDaughter(Form("%s_v0mass",name.Data()),AliRsnValueDaughter::kV0Mass);
  name.ToLower();
  if(name.Contains("k0")) a->SetBins(0.4,0.6,0.001);
  else if(name.Contains("lambda")) a->SetBins(1.08,1.16,0.001);
  else a->SetBins(0.,3.,0.01);
  AliRsnListOutput* o=new AliRsnListOutput(Form("out_%s",a->GetName()),AliRsnListOutput::kHistoDefault);
  o->AddValue(a);
  if (mon) mon->Add(o);
  if (loop) loop->AddOutput(o);
}

void AddMonitorOutput_V0DCA(TString name,TObjArray *mon,TString opt,AliRsnLoopDaughter *loop){
  AliRsnValueDaughter* a=new AliRsnValueDaughter(Form("%s_v0dca",name.Data()),AliRsnValueDaughter::kV0DCA);
  a->SetBins(0.0,0.4,0.001);
  AliRsnListOutput* o=new AliRsnListOutput(Form("out_%s",a->GetName()),AliRsnListOutput::kHistoDefault);
  o->AddValue(a);
  if (mon) mon->Add(o);
  if (loop) loop->AddOutput(o);
}

void AddMonitorOutput_V0Radius(TString name,TObjArray *mon,TString opt,AliRsnLoopDaughter *loop){
  AliRsnValueDaughter* a=new AliRsnValueDaughter(Form("%s_v0radius",name.Data()),AliRsnValueDaughter::kV0Radius);
  a->SetBins(0.0,200,0.2);
  AliRsnListOutput* o=new AliRsnListOutput(Form("out_%s",a->GetName()),AliRsnListOutput::kHistoDefault);
  o->AddValue(a);
  if (mon) mon->Add(o);
  if (loop) loop->AddOutput(o);
}

void AddMonitorOutput_V0Lifetime(TString name,TObjArray *mon,TString opt,AliRsnLoopDaughter *loop){
  AliRsnValueDaughter* a=new AliRsnValueDaughter(Form("%s_v0lifetime",name.Data()),AliRsnValueDaughter::kV0Lifetime);
  a->SetBins(0.0,200,0.1);
  AliRsnListOutput* o=new AliRsnListOutput(Form("out_%s",a->GetName()),AliRsnListOutput::kHistoDefault);
  o->AddValue(a);
  if (mon) mon->Add(o);
  if (loop) loop->AddOutput(o);
}

void AddMonitorOutput_V0DaughterDCA(TString name,TObjArray *mon,TString opt,AliRsnLoopDaughter *loop){
  AliRsnValueDaughter* a=new AliRsnValueDaughter(Form("%s_v0ddca",name.Data()),AliRsnValueDaughter::kDaughterDCA);
  a->SetBins(0.0,2,0.001);
  AliRsnListOutput* o=new AliRsnListOutput(Form("out_%s",a->GetName()),AliRsnListOutput::kHistoDefault);
  o->AddValue(a);
  if (mon) mon->Add(o);
  if (loop) loop->AddOutput(o);
}

void AddMonitorOutput_V0DCA2TPV(TString name,TObjArray *mon,TString opt,AliRsnLoopDaughter *loop){
  //DCA of secondary tracks to primary vertex
  AliRsnValueDaughter* a=new AliRsnValueDaughter(Form("%s_v0dca2tpv",name.Data()),AliRsnValueDaughter::kV0DCAXY);
  a->SetBins(-10.,10.,0.01);
  AliRsnListOutput* o=new AliRsnListOutput(Form("out_%s",a->GetName()),AliRsnListOutput::kHistoDefault);
  o->AddValue(a);
  if (mon) mon->Add(o);
  if (loop) loop->AddOutput(o);
}

void AddMonitorOutput_V0CPA(TString name,TObjArray *mon,TString opt,AliRsnLoopDaughter *loop){
  AliRsnValueDaughter* a=new AliRsnValueDaughter(Form("%s_v0cpa",name.Data()),AliRsnValueDaughter::kCosPointAng);
  a->SetBins(0.96,1.,0.001);
  AliRsnListOutput* o=new AliRsnListOutput(Form("out_%s",a->GetName()),AliRsnListOutput::kHistoDefault);
  o->AddValue(a);
  if (mon) mon->Add(o);
  if (loop) loop->AddOutput(o);
}

void AddMonitorOutput_V0TPCpim(TString name,TObjArray *mon,TString opt,AliRsnLoopDaughter *loop){
  AliRsnValueDaughter* a=new AliRsnValueDaughter(Form("%s_v0TPCpim",name.Data()),AliRsnValueDaughter::kLambdaPionPIDCut);
  a->SetBins(0.,5.,0.01);
  AliRsnListOutput* o=new AliRsnListOutput(Form("out_%s",a->GetName()),AliRsnListOutput::kHistoDefault);
  o->AddValue(a);
  if (mon) mon->Add(o);
  if (loop) loop->AddOutput(o);
}

void AddMonitorOutput_V0TPCpip(TString name,TObjArray *mon,TString opt,AliRsnLoopDaughter *loop){
  AliRsnValueDaughter* a=new AliRsnValueDaughter(Form("%s_v0TPCpip",name.Data()),AliRsnValueDaughter::kAntiLambdaAntiPionPIDCut);
  a->SetBins(-0.,5.,0.01);
  AliRsnListOutput* o=new AliRsnListOutput(Form("out_%s",a->GetName()),AliRsnListOutput::kHistoDefault);
  o->AddValue(a);
  if (mon) mon->Add(o);
  if (loop) loop->AddOutput(o);
}

void AddMonitorOutput_LambdaProtonPID(TObjArray *mon,TString opt,AliRsnLoopDaughter *lpPID){
  // Lambda Cosine of the Pointing Angle
  AliRsnValueDaughter *axisLambdaProtonPID = new AliRsnValueDaughter("lambda_protonPID", AliRsnValueDaughter::kLambdaProtonPIDCut);
  axisLambdaProtonPID->SetBins(0.0,5,0.01);

  // output: 2D histogram
  AliRsnListOutput *outMonitorLambdaProtonPID = new AliRsnListOutput("Lambda_ProtonPID", AliRsnListOutput::kHistoDefault);
  outMonitorLambdaProtonPID->AddValue(axisLambdaProtonPID);

  // add outputs to loop
  if (mon) mon->Add(outMonitorLambdaProtonPID);
  if (lpPID) lpPID->AddOutput(outMonitorLambdaProtonPID);
}

void AddMonitorOutput_LambdaAntiProtonPID(TObjArray *mon,TString opt,AliRsnLoopDaughter *lapPID){
  // Lambda Cosine of the Pointing Angle
  AliRsnValueDaughter *axisLambdaAntiProtonPID = new AliRsnValueDaughter("lambda_antiprotonPID", AliRsnValueDaughter::kAntiLambdaAntiProtonPIDCut);
  axisLambdaAntiProtonPID->SetBins(0.0,5,0.01);

  // output: 2D histogram
  AliRsnListOutput *outMonitorLambdaAntiProtonPID = new AliRsnListOutput("Lambda_AntiProtonPID", AliRsnListOutput::kHistoDefault);
  outMonitorLambdaAntiProtonPID->AddValue(axisLambdaAntiProtonPID);

  // add outputs to loop
  if (mon) mon->Add(outMonitorLambdaAntiProtonPID);
  if (lapPID) lapPID->AddOutput(outMonitorLambdaAntiProtonPID);
}
