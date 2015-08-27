#pragma once


float simplexnoise1234_noise1( float x );
float simplexnoise1234_noise2( float x, float y );
float simplexnoise1234_noise3( float x, float y, float z );
float simplexnoise1234_noise4( float x, float y, float z, float w );

float simplexnoise1234_pnoise1( float x, int px );
float simplexnoise1234_pnoise2( float x, float y, int px, int py );
float simplexnoise1234_pnoise3( float x, float y, float z, int px, int py, int pz );
float simplexnoise1234_pnoise4( float x, float y, float z, float w,
                          int px, int py, int pz, int pw );

float  simplexnoise1234_grad1( int hash, float x );
float  simplexnoise1234_grad2( int hash, float x, float y );
float  simplexnoise1234_grad3( int hash, float x, float y , float z );
float  simplexnoise1234_grad4( int hash, float x, float y, float z, float t );
