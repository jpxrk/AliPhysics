AliAnalysisTaskSignedBFMC *AddTaskSignedBFMC(TString name ="name",
					 const char* gAnalysisLevel = "AOD",
					 const char* gMultiplicityEstimator = "V0M",
					 Double_t vx = 3.,
					 Double_t vy = 3.,
					 Double_t vz = 10.,
					 Bool_t kUseOfflineTrigger = kFALSE,
					 Bool_t kCheckPileUp = kFALSE,
					 Bool_t kUseAdditionalVtxCuts = kFALSE,
					 const char* gEventPlaneDetector = "VZEROA",
					 Int_t gFilterBit = 768,
					 Double_t ptmin = 0.2, 
					 Double_t ptmax = 2.0, 

					 Double_t etamin = -0.8, 
					 Double_t etamax = 0.8) {
  //Add task for the signed BF
  //Author: Panos.Christakoglou@cern.ch
  AliAnalysisManager *mgr = AliAnalysisManager::GetAnalysisManager(); 
  
  TString fileName = AliAnalysisManager::GetCommonFileName();
  fileName+="_MyTask"; 
  
  AliAnalysisTaskSignedBFMC *taskBF = new AliAnalysisTaskSignedBFMC(name.Data()); 
  taskBF->SetAnalysisLevel(gAnalysisLevel);
  taskBF->SetMultiplicityEstimator(gMultiplicityEstimator);
  taskBF->SetVertexDiamond(vx, vy, vz);
  taskBF->SetAODtrackCutBit(gFilterBit);
  
  if(gEventPlaneDetector=="RP"){
    taskBF->SetUseReactionPlane(kTRUE);   // use Reaction plane from Model
  }
  else{
    taskBF->SetEventPlaneDetector(gEventPlaneDetector);   // use MC tracks to get Event plane. 
  }

  
  if(kUseAdditionalVtxCuts) taskBF->SetUseAdditionalVtxCuts();
  if(kUseOfflineTrigger) taskBF->UseOfflineTrigger();
  if(kCheckPileUp) taskBF->CheckPileUp();
  taskBF->SetKinematicsCutsAOD(ptmin, ptmax, etamin, etamax);
  mgr->AddTask(taskBF);

  TString outputFileName = AliAnalysisManager::GetCommonFileName();
  AliAnalysisDataContainer *coutQA = mgr->CreateContainer(Form("listQA%s",name.Data()), TList::Class(),AliAnalysisManager::kOutputContainer,outputFileName.Data());
  AliAnalysisDataContainer *coutBF = mgr->CreateContainer(Form("listBF%s",name.Data()), TList::Class(),AliAnalysisManager::kOutputContainer,outputFileName.Data());

  mgr->ConnectInput(taskBF, 0, mgr->GetCommonInputContainer());
  mgr->ConnectOutput(taskBF, 1, coutQA);
  mgr->ConnectOutput(taskBF, 2, coutBF);

  return taskBF;
}
