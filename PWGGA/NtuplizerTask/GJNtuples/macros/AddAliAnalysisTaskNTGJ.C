#ifdef __CLING__
R__ADD_INCLUDE_PATH($ALICE_PHYSICS)
#include <PWGPP/EMCAL/macros/ConfigureEMCALRecoUtils.C>
#include <OADB/macros/AddTaskPhysicsSelection.C>
#include <PWG/EMCAL/macros/AddTaskEmcalEmbeddingHelper.C>
#endif // __CLING__


AliAnalysisTaskNTGJ *
AddAliAnalysisTaskNTGJ(TString name,
                       TString emcal_correction_filename,
                       const UInt_t pSel,
                       bool physics_selection,
                       bool physics_selection_mc_analysis,
                       bool physics_selection_pileup_cut,
                       TString emcal_geometry_filename,
                       TString emcal_local2master_filename,
                       bool force_ue_subtraction,
                       double skim_cluster_min_e,
                       double skim_track_min_pt,
                       double skim_muon_track_min_pt,
                       double skim_jet_min_pt_1,
                       double skim_jet_min_pt_2,
                       double skim_jet_min_pt_3,
                       double skim_jet_average_pt,
                       int skim_multiplicity_tracklet_min_n,
                       double stored_track_min_pt,
                       double stored_jet_min_pt_raw,
                       unsigned int nrandom_isolation,
                       TString track_cuts_period,
                       bool is_mc,
                       bool is_embed,
                       TString embed_local_file_list,
                       TString embed_grid_file_pattern)
{
  AliAnalysisManager *mgr =
    AliAnalysisManager::GetAnalysisManager();

  // gROOT->LoadMacro("$ALICE_PHYSICS/OADB/macros/"
  //               "AddTaskCentrality.C");

  // AliCentralitySelectionTask *taskCentrality = AddTaskCentrality();
  // if (useMC) taskCentrality->SetMCInput();

  fprintf(stderr, "%s:%d: skim_cluster_min_e = %f\n", __FILE__,
          __LINE__, skim_cluster_min_e);

   if (is_embed) {
 #ifndef __CLING__
     gROOT->LoadMacro("$ALICE_PHYSICS/PWG/EMCAL/"
                      "macros/AddTaskEmcalEmbeddingHelper.C");
 #endif // __CLING__

     // Create the Embedding Helper
     AliAnalysisTaskEmcalEmbeddingHelper * embeddingHelper = AddTaskEmcalEmbeddingHelper();
     // Set the file pattern. This example uses ptHardBin 4 of LHC12a15e_fix.
     // The pT hard bin and anchor run can also be set by adding a printf() wild card to the string (ie %d)
     // See the documentation of AliAnalysisTaskEmcalEmbeddingHelper::GetFilenames()
     // For the grid. Include "alien://" and don't use this locally!! It will be very slow and cause strain on the grid!
     if (embed_grid_file_pattern != "") {
       embeddingHelper->SetFilePattern(embed_grid_file_pattern);
     }
     // For local use. File should be formatted the same as the normal list of input files (one filename per line).
     // Again, don't use AliEn locally!! It will be very slow and cause strain on the grid!
     if (embed_local_file_list != "") {
       embeddingHelper->SetFileListFilename(embed_local_file_list);
     }
     // If the embedded file is an ESD, then set:
     // embeddingHelper->SetESD();
     embeddingHelper->SetAOD();
     // Add additional configure as desired.
     // For some examples...
     // ... randomly select which file to start from:
     embeddingHelper->SetRandomFileAccess(kTRUE);
     // ... Start from a random event within each file
     embeddingHelper->SetRandomEventNumberAccess(kTRUE);
     // ... Set pt hard bin properties
     // embeddingHelper->SetPtHardBin(1);
     // embeddingHelper->SetNPtHardBins(11);
     // etc..
     // As your last step, always initialize the helper!
     embeddingHelper->Initialize();
   }
    

  AliAnalysisTaskNTGJ *task =
    new AliAnalysisTaskNTGJ(name.Data());

  // set to 2 to print number of clusters/tracks/MC particles
  // set to 3 to also print when entering various loops
  // AliLog::SetClassDebugLevel("AliAnalysisTaskNTGJ", 2);

  // add cluster, track, and MC containers
  if (is_embed) {
    AliClusterContainer * clusterContainer = new AliClusterContainer("caloClustersCombined");
    task->AdoptClusterContainer(clusterContainer);
  } else {
    AliClusterContainer * clusterContainer = new AliClusterContainer("usedefault");
    task->AdoptClusterContainer(clusterContainer);
  }

  // by default, AliTrackContainer uses fTrackFilterType AliEmcalTrackSelection::kHybridTracks
  // if we want to change this, we need to do SetTrackFilterType for each track container
  
  AliTrackContainer * datatrackContainer = new AliTrackContainer("usedefault");
  if (track_cuts_period != "") {
    datatrackContainer->SetTrackCutsPeriod(track_cuts_period);
    datatrackContainer->SetMinPt(0.15);
  }
  task->AdoptTrackContainer(datatrackContainer);

  if (is_embed) {
    AliTrackContainer * mctrackContainer = new AliTrackContainer("usedefault");
    mctrackContainer->SetIsEmbedding(kTRUE);
    if (track_cuts_period != "") {
      mctrackContainer->SetTrackCutsPeriod(track_cuts_period);
      mctrackContainer->SetMinPt(0.15);
    }
    task->AdoptTrackContainer(mctrackContainer);
  }

  // tracks from MC
  if (is_mc || is_embed) {
    AliMCParticleContainer * mcparticleContainer = new AliMCParticleContainer("mcparticles");
    if (is_embed) {
      mcparticleContainer->SetIsEmbedding(kTRUE);
    }
    task->AdoptMCParticleContainer(mcparticleContainer);
  }

  task->SetEMCALGeometryFilename(emcal_geometry_filename);
  task->SetEMCALLocal2MasterFilename(emcal_local2master_filename);

  if (force_ue_subtraction) {
    task->SetForceUESubtraction(force_ue_subtraction);
  }

  task->SetSkimClusterMinE(skim_cluster_min_e);
  task->SetSkimTrackMinPt(skim_track_min_pt);
  task->SetSkimMuonTrackMinPt(skim_muon_track_min_pt);
  task->SetSkimJetMinPt(skim_jet_min_pt_1, skim_jet_min_pt_2,
                        skim_jet_min_pt_3);
  task->SetSkimJetAveragePt(skim_jet_average_pt);
  task->SetSkimMultiplicityTrackletMinN(
    skim_multiplicity_tracklet_min_n);
  task->SetStoredTrackMinPt(stored_track_min_pt);
  task->SetStoredJetMinPtRaw(stored_jet_min_pt_raw);
  task->SetNRandomIsolation(nrandom_isolation);
  task->SetIsEmbed(is_embed);

  AliEMCALRecoUtils *reco_util = task->GetEMCALRecoUtils();

#ifndef __CLING__
  gROOT->LoadMacro("$ALICE_PHYSICS/PWGPP/EMCAL/macros/"
                   "ConfigureEMCALRecoUtils.C");
#endif // __CLING__

  ConfigureEMCALRecoUtils(reco_util, kFALSE, kTRUE, kTRUE,
                          kFALSE, kFALSE, kFALSE);

  reco_util->SetNumberOfCellsFromEMCALBorder(0);
  reco_util->SwitchOnRecalibration();
  reco_util->SwitchOnRunDepCorrection();

  if (physics_selection) {
#ifndef __CLING__
    gROOT->LoadMacro("$ALICE_PHYSICS/OADB/macros/"
                     "AddTaskPhysicsSelection.C");
#endif // __CLING__

    AliPhysicsSelectionTask* physics_selection_task =
      AddTaskPhysicsSelection(physics_selection_mc_analysis,
                              physics_selection_pileup_cut);

    // trying to reduce the memory
    task->SelectCollisionCandidates(pSel);
  }

  mgr->AddTask(task);
  mgr->ConnectInput(task, 0, mgr->GetCommonInputContainer());

  TString filename = mgr->GetCommonFileName();
  filename += ":AliAnalysisTaskNTGJ";
  //filename += name.Data();
    
  mgr->ConnectOutput(task, 1,
                     mgr->CreateContainer("tree", TTree::Class(),
                                          AliAnalysisManager::
                                          kOutputContainer,
                                          filename.Data()));

  return task;
}
