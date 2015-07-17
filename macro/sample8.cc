/* ----------------------------------------------------------------
   file:         sample5.cc
   description: 
   Draw 3 ESR lines like TEMPO with Dipole-dipole interaction. 
   As sample3.cc, ESR data will be fitted LineShape wrapper class.
   ---------------------------------------------------------------- */
int sample8(){

  MyApplication *app = MyApplication::instance();
  
  // ESR spectrum of TEMPO doped material
  ESRLine esr( "cal2_1.txt", 64 ); // prepare ESR data from the given file
  
  // Tunning of numerical integration parameteres.
  app->precision( 0.0001 );
  app->nGrid( 10 );
  app->nLeg( 7, 8 );
  
  // ESR lines are at 319.8, 321.3 and 322.8
  // by repeating toffset methods, the new esr line positions are added.
  // (not replacing the stored value)
  //  see sample9.cc, how to determine ESR line position
  app->toffset( 319.780 );  // esr.find( 319.7, 319.85 )
  app->toffset( 321.308 );  // esr.find( 321.2, 321.4 )
  app->toffset( 322.843 );  // esr.find( 322.75, 322.95 )

  // configure density distribution
  app->amplitude( 37.9382 );
  app->mean(       0.852397 );
  app->sigma(      0.0494269 );
  app->asym(      11.8173 );

  
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

  Fitter fitter;
  fitter.fit( g, 318.0, 325.0 );

  c->cd( 2 );
  app->drawI( 314.0, 329.0 );
  
  return 0;
}
