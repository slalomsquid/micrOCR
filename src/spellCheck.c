/*
 *
 *	Projet : Sous traitance geekou
 *	Fichier : Main.c
 *	Version 2
 *	Auteur : Vian Jeremy
 *	Date : 11/01/2013
 *
 */
 
#include "spellCheck.h"
#include <ctype.h> // For isspace()
#include <string.h>

///
///	Geekou Projet fonctions
///
///=========================================================================

// trims the spaces from the end of a string
void trim_trailing(char *str) {
    int len = strlen(str);
    
    // Iterate backward from the last character (len - 1)
    while (len > 0 && isspace(str[len - 1])) {
        len--; // Move index inward
    }
    
    // Set the new end of the string
    str[len] = '\0';
}

// copies a character string by creating a new identical string
char* copystringcharacter(const char* texteOriginal)
{
    if(texteOriginal == NULL)
        return NULL;

    char* texteCopie = (char*) malloc(sizeof(char)*(strlen(texteOriginal)+1));

    if (texteCopie == NULL) {
        return NULL;
    }
    
    strcpy(texteCopie, texteOriginal);

    texteCopie[strcspn(texteCopie, "\n")] = '\0'; 
    trim_trailing(texteCopie); 
    
    return texteCopie;
}

// main func
char* spellCheck(const char* monTexte)
{
	pDictionnaire monDico = OuvreDictionnaire("dicoFR.txt");
	if (!monDico) return (char*)monTexte;
	initSpellCheck();
	if(monTexte == NULL || monDico == NULL)	
		return NULL;
		
	char* texteOriginal = copystringcharacter(monTexte);
	// Now placed in copystringcharacter()
	// texteOriginal[strcspn(texteOriginal, "\n")] = '\0'; // Remove trailing \n
	// trim_trailing(texteOriginal);
	char* texteCorrige = copystringcharacter(monTexte);
	char* p = NULL;
	char* pCopie = NULL;
	char* pCorrige = NULL;
	char* pointeurCorrectionManuelle = NULL;
	int Mode;
	int YesNo;
	
	printf ("Correction of: \n\t '%s' \n",texteOriginal);
	p = strtok(texteOriginal,STRTOK_SYNTAX);
	while(p != NULL && strlen(p)>1 && !estUnNombre(p))
	{
		Mode = GetMode(p);
		pCopie = strdup(p);
		p = transformeEnMinuscule(p);
		//
		//	Si pas dans le dico
		//
		if(!(DicoContient(p,monDico)))
		{
			pCorrige = correct(p);
			//
			//	Si la correction est la meme que le mot
			//
			if(strcmp(p,pCorrige) == 0) {
				char tmpStr[120];
				sprintf(tmpStr, "Unable to find autocorrect %s \nEnter a correction: ",p);
				pointeurCorrectionManuelle = DemandeCorrectionManuelle(tmpStr, p);
			}
			else
			{
				YesNo = DemandeOuiouNON("Correction automatique trouvée pour '%s' => '%s' \n\tEtes vous satisfait par cette correction ? (O/N) : ", p, pCorrige);
				//
				//	La correction manuelle ne convient pas
				//
				if (YesNo)
					pointeurCorrectionManuelle = DemandeCorrectionManuelle("Entrez une correction personalisee : ",NULL);
				else
					pointeurCorrectionManuelle = strdup(pCorrige);
			}
			if(pointeurCorrectionManuelle != NULL)
			{
				texteCorrige = chercheEtRemplace(texteCorrige, pCopie, TransformeEnMode(pointeurCorrectionManuelle,Mode));
				free(pointeurCorrectionManuelle);
				pointeurCorrectionManuelle = NULL;
			}
		}
		p = strtok (NULL, STRTOK_SYNTAX);
	}
	free(texteOriginal);
	return texteCorrige;
}
int GetMode(const char* maChaine)
{
	if(estPremiereLettreMaj(maChaine))
		return 1;
	if(estEnMaj(maChaine))
		return 2;
	if(estEnMin(maChaine))
		return 3;
	return 0;
}
char* TransformeEnMode(char* maChaine,int Mode)
{
	switch(Mode)
	{
		case 1:
			return transformeEnPremiereLettreMajuscule(maChaine);
			break;
			
		case 2:
			return transformeEnMajuscule(maChaine);
			break;
			
		case 3:
			return transformeEnMinuscule(maChaine);
			break;
			
		default:
			return maChaine;
			break;
	}
}
///
///	DemandeCorrectionManuelle()
///
///		Mode 0 : On ne cherche pas a comprendre
///		Mode 1 : Premiere Lettre en Majuscule
/// 	Mode 2 : En Majuscule
///		Mode 3 : En Minuscule
///===========================================================================
char* DemandeCorrectionManuelle(const char* MessageDemande,char* pMot)
{
	char* pointeurCorrectionManuelle = NULL;
	char CorrectionManuelle[TAILLE_MOT_MAX];
	char Buffer[TAILLE_BUFFER];
	//va_list args;
	//va_start(args, MessageDemande);
	sprintf(Buffer,MessageDemande, pMot);
	//printf("MOT : %s \n",pMot);
	//va_end(args);

	printf("%s",MessageDemande);
	//scanf("%s",CorrectionManuelle);
	// gets(CorrectionManuelle);
	fgets(CorrectionManuelle, sizeof(CorrectionManuelle), stdin); //Replace to prevent overflow and warning
	// CorrectionManuelle[strcspn(CorrectionManuelle, "\n")] = '\0'; // Remove trailing \n left by fgets // Not needed
	pointeurCorrectionManuelle = copystringcharacter(CorrectionManuelle);

	return pointeurCorrectionManuelle;
}
int DemandeOuiouNON(const char *chaine, ...)
{
	char DemandeChoix[TAILLE_CHOIX_MAX];
	char Buffer[TAILLE_BUFFER];
	va_list args;
	va_start(args, chaine);
	vsprintf(Buffer,chaine, args);
	va_end(args);
	
	do
	{
		printf("%s",Buffer);
		gets(DemandeChoix);
		//scanf("%s",DemandeChoix);
	} while ((strcmp(DemandeChoix,"O") != 0) && (strcmp(DemandeChoix,"N") != 0));
	
	if(strcmp(DemandeChoix,"O") == 0 || strcmp(DemandeChoix,"Oui") == 0)
		return 0;
	if(strcmp(DemandeChoix,"N") == 0 || strcmp(DemandeChoix,"Non") == 0)
		return 1;
	return 1;
}

///
///	Dictionnaire PART
///
///=========================================================================
pDictionnaire OuvreDictionnaire(const char* nomFichier)
{
	if(nomFichier == NULL)
		return NULL;
	pFILE monFichier;
	monFichier = fopen(nomFichier,"r");
	
	pDictionnaire monDico= (pDictionnaire) malloc(sizeof(Dictionnaire));
	monDico->NombreMots = LisNombreDeMots(monFichier);
	monDico->LeMot = (char**)malloc(sizeof(char*)*monDico->NombreMots );
	uint i;
	for(i = 0; i < monDico->NombreMots ; i++)
		monDico->LeMot[i] = LisMot(monFichier);
	
	return monDico;
}

void FermeDictionnaire(pDictionnaire monDictionnaire)
{
	if(monDictionnaire == NULL)
		return ;
	uint i;
	for(i = 0 ; i < monDictionnaire->NombreMots ; i++)
		free(monDictionnaire->LeMot[i]);
	free(monDictionnaire);
}

bool DicoContient(const char* Mot, pDictionnaire monDictionnaire)
{
	if(Mot == NULL || monDictionnaire == NULL)
		return NULL;
	uint i;
	for(i = 0 ; i < monDictionnaire->NombreMots; i++)
		if(strcmp(Mot,monDictionnaire->LeMot[i]) == 0)
			return true;
	return false;
}

uint LisNombreDeMots(pFILE monFichier)
{
	if(monFichier == NULL)
		return 0;
	uint NombreDeMots = 0;
	fscanf(monFichier, "%u ", &NombreDeMots);
	return NombreDeMots;
}

char* LisMot(pFILE monFichier)
{
	if(monFichier == NULL)
		return NULL;
	char motDico[TAILLE_MOT_MAX];
	fscanf(monFichier,"%s\n",(char*)motDico);
	char* monMot = (char*) malloc(sizeof(char)*(strlen(motDico)+1));
	strcpy(monMot,motDico);
	return monMot;
}

///
///	Operations on character strings
///
///=========================================================================
char* chercheEtRemplace(char* maChaine, const char* ARemplacer, const char* RemplacePar)
{  

	char* replacement_copy = strdup(RemplacePar);

	if (replacement_copy == NULL) {
        return NULL; // Return NULL or handle error
    }

	char* tmp = maChaine; 
	char* result; 
	int   found = 0; 
	int   length, reslen;

	replacement_copy[strcspn(replacement_copy, "\n")] = '\0'; // Remove \n

	int newlen = strlen(replacement_copy);
	int   oldlen = strlen(ARemplacer); 
	
	trim_trailing(RemplacePar);

	printf("Replacing '%s' with '%s' in '%s' \n",ARemplacer,RemplacePar,maChaine);
	while ((tmp = strstr(tmp, ARemplacer)) != NULL) 
		found++, tmp += oldlen;
		
	//printf("chercheEtRemplace() : found = %d\n",found);

	length = strlen(maChaine) + found * (newlen - oldlen); 
	if ( (result = (char *)malloc(length+1)) == NULL)
		printf("searchAndReplace(): Not enough memory!\n");
	else
	{ 
		tmp = maChaine; 
		reslen = 0;
		while ((tmp = strstr(tmp, ARemplacer)) != NULL)
		{ 
			length = (tmp - maChaine);
			strncpy(result + reslen, maChaine, length);
			strcpy(result + (reslen += length), RemplacePar);
			reslen += newlen; 
			tmp += oldlen; 
			maChaine = tmp;
		} 
		strcpy(result + reslen, maChaine);
	} 
	return result; 
} 

///
///	Transforms a character string into lowercase then deletes the original string from memory
///
char* transformeEnMinuscule(char* maChaine)
{
	if(maChaine == NULL)
		return NULL;
	uint i;
	for(i = 0 ; i < strlen(maChaine); i++)
		if(isupper(maChaine[i]))
			maChaine[i] = tolower(maChaine[i]);
	return maChaine;
}

char* transformeEnMinusculeSansToucher(const char* maChaine)
{
	if(maChaine == NULL)
		return NULL;
	uint i;
	char* EnMIN = copystringcharacter(maChaine);
	for(i = 0 ; i < strlen(maChaine); i++)
		if(isupper(maChaine[i]))
			EnMIN[i] = tolower(maChaine[i]);
	return EnMIN;
}

char* transformeEnMajuscule(char* maChaine)
{
	if(maChaine == NULL)
		return NULL;
	uint i;
	for(i = 0 ; i < strlen(maChaine); i++)
		if(islower(maChaine[i]))
			maChaine[i] = toupper(maChaine[i]);
	return maChaine;
}

char* transformeEnPremiereLettreMajuscule(char* maChaine)
{
	if(maChaine == NULL)
		return NULL;
	uint i;
	for(i = 0 ; i < strlen(maChaine); i++)
	{
		if(i == 0 && islower(maChaine[i]))
			maChaine[i] = toupper(maChaine[i]);
		else if(isupper(maChaine[i]))
			maChaine[i] = tolower(maChaine[i]);
	}
	return maChaine;
}

int estUnNombre(const char * s)
{
    if (s == NULL || *s == '\0' || isspace(*s))
      return 0;
    char * p;
    strtod (s, &p);
    return *p == '\0';
}

int estPremiereLettreMaj(const char* maChaine)
{
	if(maChaine == NULL)
		return 0;
	return isupper(maChaine[0]);
}

int estEnMaj(const char* maChaine)
{
	if(maChaine == NULL)
		return 0;
	uint i;
	for(i = 0 ; i < strlen(maChaine) + 1 ; i++)
		if(islower(maChaine[i]))
			return 0;
	return 1;
}

int estEnMin(const char* maChaine)
{
	if(maChaine == NULL)
		return 0;
	uint i;
	for(i = 0 ; i < strlen(maChaine) + 1 ; i++)
		if(isupper(maChaine[i]))
			return 0;
	return 1;
}
