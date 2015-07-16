/* ----------------------------------------------------------------
   file:         sample3.cc
   description: 
   Example for ESR data fitting. The parameters of asymmetric gausian 
   will be determined by fitting the ESR data.
   ---------------------------------------------------------------- */
int sample7(){

  MyApplication *app = MyApplication::instance();

  // In order to reduce computation time, data point will be sampled
  // here one over 64 points will be used.
  ESR esr( "cofeebean-a.txt", 32 );

  // Tunning of numerical integration parameteres.
  app->precision( 0.0001 );
  app->nGrid( 10 );
  app->nLeg( 7, 8 );

  app->toffset( 328.87 );
  //  app->amplitude( 80.2102 );
  app->amplitude( 50 );
  app->mean( 1.00307 );
  app->sigma( 0.0161825 );
  app->asym( 24.2989 );
  app->draw( &esr );
  
  gPad->Update();
  
  //  double sig[2] = { 326.9, 330.9 };
  double sig[2] = { 327.5, 330.3 };
  
  //
  //  Data preparation
  //
  TGraph* g = (TGraph*) esr.GetGraphInteg()->Clone();
  
  // background shape determination
  double bg[2][2] = { {324.6, 326.0 }, {331.6, 332.1 } };
  
  // prepare background data
  TGraph *gBG = new TGraph;
  for( int i = 0; i < g->GetN(); i++ ){
    double x, y;
    g->GetPoint( i, x, y );
    if( ( x > bg[ 0 ][ 0 ] && x < bg[ 0 ][ 1 ] ) ||
	( x > bg[ 1 ][ 0 ] && x < bg[ 1 ][ 1 ] ) ){
      int iNew = gBG->GetN();
      gBG->Set( iNew + 1 );
      gBG->SetPoint( iNew, x, y );
    }
  }
  
  gBG->SetMarkerColor( kOrange );
  gBG->SetMarkerStyle( 20 );
  gBG->Draw( "P" );
  gPad->Update();
  
  // Parametize background shape with 2nd polynominal.
  // Parameters will be determine by fitting tails of the ESR spectrum
  TF1 *fBG = new TF1( "fBG", "pol2", sig[ 0 ], sig[ 1 ] );
  gBG->Fit( fBG, "N"  );
  
  // signal data preparation
  // backgound contribution will be subtracted using the fitted bachground shape
  TGraph *gSig = new TGraph;
  for( int i = 0; i < g->GetN(); i++ ){
    double x, y;
    g->GetPoint( i, x, y );
    if(  x < sig[ 0 ] || x > sig[ 1 ] ) continue;
    int iNew = gSig->GetN();
    gSig->Set( iNew + 1 );
    gSig->SetPoint( iNew, x, y - fBG->Eval( x ) );
  }
  
  gSig->SetMarkerStyle( 20 );
  gSig->SetMarkerColor( kCyan );
  gSig->Draw( "P" ); 
  gPad->Update();
  
  Fitter fitter;
  fitter.fit( gSig );
  
  
  app->draw();
  gSig->Draw( "SAMEp" );
  
  return 0;
}
