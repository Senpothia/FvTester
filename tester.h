// Fonctions générales

void initialConditions(bool *, bool *, bool *);
bool testOK(bool active);
bool testNOK(bool active);
void ledNonConforme(bool active);
void ledConforme(bool active);
void ledProgession(bool active);
void alerteDefaut(char etape[], bool *, bool *);
bool reponseOperateur(bool automatique);
void activerBuzzer();
void startAlert(void);
void errorAlert(void);
void okAlert(void);
void attenteDemarrage(bool *, bool *);
void attenteAquittement(bool *, bool *);
void sortieErreur(bool *, bool *,bool *);

// Fonctions particulières
bool testRelais(bool active);
void rasAlimention(void);
void IN12(void);
void IN48(void);
void TC12(void);
void TC48(void);
void IN_OFF(void);
void TC_OFF(void);
void modeBP(bool active);
bool testFermeture(bool active);
void activerReed(bool active);
void activerBP(bool active);
int testCP(void);
void debloquerCPs(void);
void attenteOK(void);


