/* ----------------------------------------------------------------
   file:         sample6.cc
   description: 
   Test of matrix conversion
   ---------------------------------------------------------------- */
int sample6(){

  MyApplication *app = MyApplication::instance();

  // In order to reduce computation time, data point will be sampled
  // here one over 64 points will be used.
  ESR esr( "cofeebean-a.txt", 2 );
  
  // Tunning of numerical integration parameteres.
  app->precision( 0.0001 );
  app->nGrid( 10 );
  app->nLeg( 7, 8 );

  // copy resutls of sample3
  app->amplitude( 152.0 );
  app->mean( 1.025 );
  app->sigma( 0.0425 );
  app->asym( 9.0 );
  app->toffset( 328.875 );
  //  app->draw( &esr );
  
  double sig[2] = { 326.9, 330.9 };
  
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

  TCanvas *c = new TCanvas( "c1", "ESR", 794, 1123 );
  c->Divide( 1, 2 );
  c->cd( 1 );
  
  gBG->SetMarkerColor( kOrange );
  gBG->SetMarkerStyle( 20 );
  //  gBG->Draw( "P" );
  
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
  gSig->Draw( "AP" ); 

  c->Update();
  
  int nT = gSig->GetN();
  AGaus& aG = *(app->rho());
  
  // data preparation:
  // rho(r)
  int nR = gSig->GetN();  // make Kernel Matrix square
  double minR = 0.1;
  double maxR = 5.0;
  double dR = ( maxR - minR ) / nR;
  TMatrixT< double > rho( nR, 1 );
  for( int iR = 0; iR < nR; iR++ ){
    double r = minR + dR * iR;
    rho[ iR ][ 0 ] = app->kernel()->weight( r ) * aG( r ) * dR;
  }

  // K( r, t )
  TMatrixT< double > K( nT, nR );
  TMatrixT< double > rawI( nT, 1 );
  for( int iT = 0; iT < nT; iT++ ){
    double t;
    double val;
    gSig->GetPoint( iT, t, val );
    rawI[ iT ][ 0 ] = val;
    for( int iR = 0; iR < nR; iR++ ){
      double r = minR + dR * iR;
      K[ iT ][ iR ] = app->kernel()->core( r, t );
    }
  }
  
  // I = K * rho
  TMatrixT< double > calI = K * rho;
  TGraph *gI = new TGraph( nT );
  for( int iT = 0; iT < nT; iT++ ){
    double t;
    double val;
    gSig->GetPoint( iT, t, val );
    gI->SetPoint( iT, t, calI[ iT ][ 0 ] );
  }
  
  gI->SetLineColor( kRed );
  gI->SetLineWidth( 2 );
  gI->SetLineStyle( 1 );
  gI->Draw( "SAMEL" );
  c->Update();
  
  TMatrixT< double > Kinv = K;
  Kinv.Invert();


  TMatrixT< double > calRho = Kinv * rawI;
  TGraph *gCalRho = new TGraph( nR );
  
  for( int iR = 0; iR < nR; iR++ ){
    double r = minR + dR * iR;
    calRho[ iR ][ 0 ] /= app->kernel()->weight( r ) * dR ;
    gCalRho->SetPoint( iR, r, calRho[ iR ][ 0 ] );
  }

  c->cd( 2 );
  gCalRho->Draw( "Al" );
    
  // Full area: 324.6 - 332.0
  // signal center: 328.9
  // Signal area: 326.9 - 330.9

  // fit the ESR signal with lineShape object
  //  gSig->Fit( f1, "N", "", sig[0], sig[1] );
  
  //  app->draw();
  
  return 0;
}
