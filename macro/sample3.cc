/* ----------------------------------------------------------------
   file:         sample3.cc
   description: 
   Example for ESR data fitting. The parameters of asymmetric gausian 
   will be determined by fitting the ESR data.
   ---------------------------------------------------------------- */
int sample3(){

  MyApplication *app = MyApplication::instance();

  // In order to reduce computation time, data point will be sampled
  // here one over 64 points will be used.
  ESR esr( "cofeebean-a.txt", 64 );

  // Tunning of numerical integration parameteres.
  app->precision( 0.0001 );
  app->nGrid( 10 );
  app->nLeg( 7, 8 );
  
  app->amplitude( 152.0 );
  app->mean( 1.025 );
  app->sigma( 0.0425 );
  app->asym( 9.0 );
  app->draw( &esr );

  double sig[2] = { 326.9, 330.9 };
  
  // get wrapper class object for TF1
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
  f1->Draw( "SAME" );

  
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
  
  // Full area: 324.6 - 332.0
  // signal center: 328.9
  // Signal area: 326.9 - 330.9

  // fit the ESR signal with lineShape object
  gSig->Fit( f1, "N", "", sig[0], sig[1] );
  
  //  app->draw();
  
  return 0;
}
