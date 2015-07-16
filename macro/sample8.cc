/* ----------------------------------------------------------------
   file:         sample5.cc
   description: 
   Draw 3 ESR lines like TEMPO with Dipole-dipole interaction. 
   As sample3.cc, ESR data will be fitted LineShape wrapper class.
   ---------------------------------------------------------------- */
int sample5(){

  MyApplication *app = MyApplication::instance();

  // ESR spectrum of TEMPO doped material
  ESR esr( "cal2_1.txt", 64 ); // prepare ESR data from the given file

  // Tunning of numerical integration parameteres.
  app->precision( 0.0001 );
  app->nGrid( 10 );
  app->nLeg( 7, 8 );
  
  // ESR lines are at 319.8, 321.3 and 322.8
  // by repeating toffset methods, the new esr line positions are added.
  // (not replacing the stored value)
  app->toffset( 319.75 );
  app->toffset( 321.3 );
  app->toffset( 322.84 );

  // configure density distribution
  app->amplitude( 47.1337 );
  app->mean( 0.9038 );
  app->sigma( 0.048 );
  app->asym( 11.68 );

  // get wrapper class object for TF1
  double sig[2] = { 318.0, 325.0 };
  LineShape* lS = app->lineShapeObj();
  TF1 *f1 = new TF1( "lineShape", lS, sig[0], sig[1], 5, "LineShape" );
  // set initial values
  f1->SetParameter( 0, app->amplitude() );
  f1->SetParameter( 1, app->mean() );
  f1->SetParameter( 2, app->sigma() );
  f1->SetParameter( 3, app->asym()  );
  f1->FixParameter( 4, 0.0   );            // offset constant

  // style setting
  f1->SetLineColor( kMagenta );
  f1->SetLineStyle( 2 );
  f1->SetLineWidth( 2 );
  
  TCanvas *c = new TCanvas( "c1", "ESR", 794, 1123 );
  c->Divide( 1, 3 );
  c->cd( 1 );
  app->drawRho();

  c->cd( 3 );
  esr.GetGraph( true )->DrawClone( "Al" );

  c->cd( 2 );
  app->drawI( &esr );

  c->Update();
  //
  //  Data preparation
  //
  TGraph* g = (TGraph*) esr.GetGraphInteg()->Clone();
  g->Fit( f1, "VNME", "", sig[0], sig[1] );

  c->cd( 2 );
  app->drawI( 314.0, 329.0 );
  
  return 0;
}
