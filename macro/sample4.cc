int sample4(){
  // get a pointer to the application object
  MyApplication *app = dynamic_cast< MyApplication* >( gROOT->GetApplication() );

  // configure transfer parameters
  app->precision( 0.0001 );
  app->nGrid( 10 );
  app->nLeg( 7, 8 );

  // configure density distribution
  app->amplitude( 152.0 );
  app->mean( 1.025 );
  app->sigma( 0.0425 );
  app->asym( 9.0 );

  // prepare ESR data
  ESR esr( "cofeebean-a.txt", 64 );

  TCanvas* c= new TCanvas;

  c->Divide( 2, 2 );

  c->cd( 1 );
  app->drawI();

  c->cd( 2 );
  app->drawRho();

  c->cd( 3 );
  TGraph* g = (TGraph*) esr.GetGraphInteg()->Clone();
  g->Draw( "Al" );
  
  
  
  return 0;
}
