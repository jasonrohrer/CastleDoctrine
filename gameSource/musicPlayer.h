

// dimensions of note grid
#define N  16


// instrument parts
// one for each enemy (10), one for each power-up token (10),
// two for placed flags
// two for rise marker,
// one for player, and one for whatever we're inside

#define PARTS 26

// last part (PARTS-1) doesn't have its own timbre/envelope, but instead
// uses pointers to one of other part's timber/envelope
extern char noteToggles[PARTS][N][N];
extern int partLengths[PARTS];

extern double partLoudness[PARTS];
extern double partStereo[PARTS];




void initMusicPlayer();
void freeMusicPlayer();


// sets the part that the last part copies for timbre/envelope
void setCopiedPart( int inPartIndex );




// set loudness in range [0.0,1.0]
void setMusicLoudness( double inLoudness );


double getMusicLoudness();


// returns all timbres/evelopes to those set by default
void setDefaultMusicSounds();


// inNumCoefficients must not exceed 256
void setTimbre( int inTimbreNumber, 
                double *inPartialCoefficients, int numCoefficients,
                int inOctavesDown );




// release + attack + hold must be <= 1.0
// sustains at full volume between attack and release slopes (no decay)
void setEnvelope( int inTimbreNumber,
                  double inAttack, double inHold,
                  double inRelease );



// sets scale 
// NOTE:  all timbres must be regenerated for this to have an effect
void setScale( char inToneOn[12] );


// 0, 1, 2, slow, med, fast    
// NOTE:  all envelopes must be regenerated for this to 
// have an effect
void setSpeed( int inSpeed );



// causes music to jump back to beginning
void restartMusic();



void stopMusic();

