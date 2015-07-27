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
  app->amplitude( 34.4723 );
  app->mean( 0.947862 );
  AGaus *ag = dynamic_cast< AGaus* >( app->density() );
  if( ag ){
    ag->asigma( true,  0.5469354 );
    ag->asigma( false, 0.0497214 );
    app->update();
  }
  
  TCanvas *c = new TCanvas( "c1", "ESR", 794, 1123 );
  c->Divide( 1, 3 );
  c->cd( 1 );
  app->drawRho();

  c->cd( 3 );
  esr.GetGraph( true )->DrawClone( "Al" );

  c->cd( 2 );
  app->drawI( &esr );

  double fitR[2] = { 318.5, 324.0 };
  TLine *line = new TLine;
  line->SetLineWidth( 2 );
  line->SetLineColor( kMagenta );
  line->DrawLine( fitR[ 0 ], 0, fitR[ 0 ], 500 );
  line->DrawLine( fitR[ 1 ], 0, fitR[ 1 ], 500 );
  
  c->Update();

  //
  //  Data preparation
  //
  TGraph* g = (TGraph*) esr.GetGraphInteg()->Clone();

  Fitter fitter;
  //  fitter.FixParameter( 0 );
  //  fitter.FixParameter( 1 );
  //  fitter.FixParameter( 2 );
  //  fitter.FixParameter( 3 );
  fitter.fit( g, fitR[ 0 ], fitR[ 1 ] );

  c->cd( 2 );
  app->drawI( 314.0, 329.0 );
  
  return 0;
}
