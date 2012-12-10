

typedef struct GridPos {
        int x;
        int y;
    } GridPos;



// returns true if path found
// if outNumStepsToGoal = 0, start and goal equal, and outFullPath set to NULL
// outFullPath destroyed by caller if not NULL
char pathFind( int inMapH, int inMapW,
               char *inBlockedMap, 
               GridPos inStart, GridPos inGoal, 
               int *outNumStepsToGoal,
               GridPos **outFullPath );

