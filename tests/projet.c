/**
 * \file projet.c
 * \brief Code Source du projet d'algo 2012-2013
 * \author Adrien Bertrand (Groupe B2 avec Romain Bosselet et Yanis Chamieh)
 * \version 2.7
 * \date 26/01/2013
 *
 * Fichier .c contenant l'implémentation du programme de reconnaissance matricielle de caractères
 *
 */

 // Toute la documentation se trouve dans le fichier projet.h, optimisée pour Doxygen (voir dossier ./doc/)
 
#include "projet.h"
#include "spellCheck.h" // spellChecker

long long int getIfromXYinImage(DonneesImageRGB* image, int x, int y) {
    return (x>=0 && y>=0 && x<image->largeurImage && y<image->hauteurImage) ? (3 * (x + y * image->largeurImage)) : -1;
}

Couleur getColorAt(DonneesImageRGB* image, int x, int y) {
	long long int i = getIfromXYinImage(image, x ,y);
	return (i==-1) ? (Couleur){255,255,255} : getColorAtI(image, i); // blanc supposé par défaut.
}

Couleur getColorAtI(DonneesImageRGB* image, int i) {
    return (Couleur){ image->donneesRGB[i+2], image->donneesRGB[i+1], image->donneesRGB[i] };
}

int CouleursEgales(Couleur c1, Couleur c2) {
    return memcmp(&c1, &c2, sizeof(Couleur))==0;
    //return (c1.r == c2.r && c1.g == c2.g && c1.b == c2.b);
}

int indiceFreqMax(colorCount* tab, int taille) {
    int i, indice = 0;
    for (i=0; i<taille; i++)
        indice = tab[indice].freq < tab[i].freq ? i : indice;
    return indice;
}

int indiceFreqSousMax(colorCount* tab, int taille, int except) {
    int i, indice = 1;
    for (i=0; i<taille; i++)
        indice = ((tab[indice].freq < tab[i].freq) && i!=except) ? i : indice;
    return indice;
}

int getRectangleSurface(Rectangle rect) {
    return rect.w*rect.h;
}

void drawRect(int x1, int y1, int x2, int y2) { // draws an empty rect
    ligne(x1, y1, x2, y1);
    ligne(x2, y1, x2, y2);
    ligne(x2, y2, x1, y2);
    ligne(x1, y2, x1, y1);
}

int strEndsWith(const char *str, const char *suffix) {
    if (!str || !suffix)
        return 0;
    size_t lenstr = strlen(str);
    size_t lensuffix = strlen(suffix);
    if (lensuffix > lenstr)
        return 0;
    return strncmp(str + lenstr - lensuffix, suffix, lensuffix) == 0;
}

// from the public domain source code on http://ndevilla.free.fr/median/median/
int quick_select(int* arr, int n) 
{
    int low, high;
    int median;
    int middle, ll, hh;

    low = 0; high = n-1; median = (low + high) / 2;
    for (;;) {
        if (high <= low) /* One element only */
            return arr[median];

        if (high == low + 1) {  /* Two elements only */
            if (arr[low] > arr[high])
                INTSWAP(arr[low], arr[high]);
            return arr[median];
        }

    /* Find median of low, middle and high items; swap into position low */
    middle = (low + high) / 2;
    if (arr[middle] > arr[high])    INTSWAP(arr[middle], arr[high]);
    if (arr[low] > arr[high])       INTSWAP(arr[low], arr[high]);
    if (arr[middle] > arr[low])     INTSWAP(arr[middle], arr[low]);

    /* Swap low item (now in position middle) into position (low+1) */
    INTSWAP(arr[middle], arr[low+1]);

    /* Nibble from each end towards middle, swapping items when stuck */
    ll = low + 1;
    hh = high;
    for (;;) {
        do ll++; while (arr[low] > arr[ll]);
        do hh--; while (arr[hh]  > arr[low]);

        if (hh < ll)
        break;

        INTSWAP(arr[ll], arr[hh]);
    }

    /* Swap middle item (in position low) back into correct position */
    INTSWAP(arr[low], arr[hh]);

    /* Re-set active partition */
    if (hh <= median)
        low = ll;
        if (hh >= median)
        high = hh - 1;
    }
}

// from the public domain source code on http://rosettacode.org/wiki/Sorting_algorithms/Quicksort#C
void quickSort(int *a, int n) {
    if (n < 2)
        return;
    int p = a[n / 2];
    int *l = a;
    int *r = a + n - 1;
    while (l <= r) {
        if (*l < p) {
            l++;
            continue;
        }
        if (*r > p) {
            r--;
            continue; // we need to check the condition (l <= r) every time we change the value of l or r
        }
        int t = *l;
        *l++ = *r;
        *r-- = t;
    }
    quickSort(a, r - a + 1);
    quickSort(l, a + n - l);
}
 
void dispTab(int *tab, int nbrElem) {
	int i;
	for (i=0; i<nbrElem; i++)
		printf("%d\t", tab[i]);
	printf("\n");
}

Rectangle getMinRect(DonneesImageRGB* image, Couleur couleurTexte) {
    int width = image->largeurImage;
    int height = image->hauteurImage;

    int x,y;
    int xmin=-1, xmax=-1, ymin=-1, ymax=-1;

    for (y=1; y<height && ymin<0; y++)
        for (x=1; x<width && ymin<0; x++)
            if (CouleursEgales(getColorAt(image, x, y), couleurTexte))
                ymin = y;
    for (y=height-1; y>ymin && ymax<0; y--)
        for (x=1; x<width && ymax<0; x++)
            if (CouleursEgales(getColorAt(image, x, y), couleurTexte))
                ymax = y;
    for (x=0; x<width && xmin<0; x++)
        for (y=ymax; y>ymin && xmin<0; y--)
            if (CouleursEgales(getColorAt(image, x, y), couleurTexte))
                xmin = x;
    for (x=width; x>=xmin && xmax<0; x--)
        for (y=ymax; y>ymin && xmax<0; y--)
            if (CouleursEgales(getColorAt(image, x, y), couleurTexte))
                xmax = x;

    Rectangle rect = { xmin, ymin, xmax, ymax, abs(xmax-xmin), abs(ymax-ymin) };
    return rect;
}

DonneesImageRGB* improveImage(DonneesImageRGB* image) {
	// Todo.
	unsigned int width = (unsigned int)image->largeurImage;
    unsigned int height = (unsigned int)image->hauteurImage;
    unsigned int i;

    unsigned int x,y;
    
    // niveaux de gris (average)
    for (i=0; i<3*(width*height); i+=3)
		image->donneesRGB[i] = image->donneesRGB[i+1] = image->donneesRGB[i+2] = (image->donneesRGB[i] + image->donneesRGB[i+1] + image->donneesRGB[i+2]) / 3;
	
	int* tab9Couleurs = calloc(9,sizeof(int));
	int tmpi;
    // filtre bruit (median)
    for (y=0; y<height; y++) {
        for (x=0; x<width; x++) {
			//printf("%d %d\n", y, x);
            // yeah, it's ugly...
            tab9Couleurs[0] = getColorAt(image, x-1, y-1).r;
            tab9Couleurs[1] = getColorAt(image, x, y-1).r;
            tab9Couleurs[2] = getColorAt(image, x+1, y-1).r;
            tab9Couleurs[3] = getColorAt(image, x-1, y).r;
            tab9Couleurs[4] = getColorAt(image, x, y).r;
            tab9Couleurs[5] = getColorAt(image, x+1, y).r;
            tab9Couleurs[6] = getColorAt(image, x-1, y+1).r;
            tab9Couleurs[7] = getColorAt(image, x, y+1).r;
            tab9Couleurs[8] = getColorAt(image, x+1, y+1).r;
			//dispTab(tab9Couleurs, 9);
            tmpi = getIfromXYinImage(image, x, y);
            image->donneesRGB[tmpi] = image->donneesRGB[tmpi+1] = image->donneesRGB[tmpi+2] = quick_select(tab9Couleurs, 9);  // calcul de médiane
		}
	}
	
	free(tab9Couleurs);
    
    //reduction de la palette /  "Arrondi" des tons de couleurs.
    /*for (i=0; i<3*(width*height); i+=3) {
		image->donneesRGB[i] = image->donneesRGB[i] - (image->donneesRGB[i] % 20);
		image->donneesRGB[i+1] = image->donneesRGB[i+1] - (image->donneesRGB[i+1] % 20);
		image->donneesRGB[i+2] = image->donneesRGB[i+2] - (image->donneesRGB[i+2] % 20);
	}*/
	
	//printf("***debug*** Image improved.\n");

	return image;
}

DonneesImageRGB* rotateImage(DonneesImageRGB* image, int angle_degre, Couleur couleurFond) {

    double angle = -angle_degre*M_PI/180; // radian
    double sinangle = sin(angle);
    double cosangle = cos(angle);

    int width = (unsigned int)image->largeurImage;
    int height = (unsigned int)image->hauteurImage;

    int newWidth = (unsigned int)round(width*fabs(cosangle)+height*fabs(sinangle));
    int newHeight = (unsigned int)round(width*fabs(sinangle)+height*fabs(cosangle));

    DonneesImageRGB *rotatedImg = (DonneesImageRGB*)calloc(1, sizeof(DonneesImageRGB));
    if (!rotatedImg) {
        printf("Impossible de malloc la rotatedImg. Fermeture du programme !\n");
        exit(-1);
    }
    rotatedImg->largeurImage = newWidth;
    rotatedImg->hauteurImage = newHeight;
    rotatedImg->donneesRGB = (unsigned char *)calloc(3*newWidth*newHeight,1);
    if (!rotatedImg->donneesRGB) {
        printf("Impossible de malloc la rotatedImg->donneesRGB. Fermeture du programme !\n");
        exit(-1);
    }

    int i,x,y;

    int hwidth = newWidth / 2;
    int hheight = newHeight / 2;

    for (i=0; i < 3*(newWidth*newHeight); i=i+3) {

        x = (i/3)%newWidth;
        y = (i/3)/newWidth;

        int xt = x - hwidth;
        int yt = y - hheight;

        int xs = (int)round((cosangle * xt + sinangle * yt) + .5*width);
        int ys = (int)round((-sinangle * xt + cosangle * yt) + .5*height);

        if(xs >= 0 && xs < width && ys >= 0 && ys < height) {
            Couleur oldColor = getColorAt(image, xs, ys);
            rotatedImg->donneesRGB[i] = oldColor.b;   // b
            rotatedImg->donneesRGB[i+1] = oldColor.g; // g
            rotatedImg->donneesRGB[i+2] = oldColor.r; // r
        } else {
            rotatedImg->donneesRGB[i] = couleurFond.b; // b
            rotatedImg->donneesRGB[i+1] = couleurFond.g; // g
            rotatedImg->donneesRGB[i+2] = couleurFond.r; // r
        }
    }
	
    return rotatedImg;
}

int findBestAngle(DonneesImageRGB* image, Couleur couleurFond, Couleur couleurTexte) {
    int angle = 0;
    int i;
    DonneesImageRGB* tmpImage;
    unsigned long int surfaceTmp = 999999999;
    unsigned long int surface = 999999999;

    //printf("***debug***   ------Debut de la minimalisation de la surface par rotation------\n");
    for (i=0; i<180; i++)
    {
        tmpImage = rotateImage(image, i, couleurFond);
        Rectangle tmpMinRect = getMinRect(tmpImage, couleurTexte);
        surfaceTmp = getRectangleSurface(tmpMinRect);
        if ((surfaceTmp < surface) && (tmpMinRect.w > tmpMinRect.h)) { //horizontalité forcée (du coup, attention : ca peut etre a l'envers de 180°)
            //printf("***debug*** Amelioration surface trouvee (%lu) pour angle = %d\n", surfaceTmp, i);
            surface = surfaceTmp;
            angle = i;
        } else {
            //printf("***debug*** Surface non-optimale (%lu) pour angle = %d\n", surfaceTmp, i);
        }
		freeImageRGB(tmpImage);
    }
    if (angle>90) {
        printf("*Note* : Reajustement de l'angle (angle-180) pour cause probable de renversement intentionnel\n");
        angle -= 180; // si c'était bien, ca sera traitée par la suite par détection d'erreur (non-détection de lettres)
    }
    //printf("* Meilleur angle de rotation trouve : %d\n", angle);
    //printf("***debug***   ------Fin de la minimalisation de la surface par rotation------\n");

    return angle;
}

void getBackgroundAndTextColor(DonneesImageRGB* image, Couleur* fond, Couleur* texte) {
    colorCount* tabCouleurs = (colorCount*)calloc(100,sizeof(colorCount));
    // tableau de frequences des Couleurs de l'image, 100 par défaut - realloc de toute facon.
    if (!tabCouleurs) {
        printf("Impossible de malloc tabCouleurs. Fermeture du programme !\n");
        exit(-1);
    }

    int i,j,colonneEcriture=0,trouve;
    int tailleTabCouleurs = 100;

    for (i=0; i<3*(image->largeurImage*image->hauteurImage); i=i+3) {
        int currentB = image->donneesRGB[i];
        int currentG = image->donneesRGB[i+1];
        int currentR = image->donneesRGB[i+2];

        // si gris, on passe en noir ou blanc selon le degré de gris.
        if ((currentB == currentG) && (currentB == currentR)) {
            currentB = currentG = currentR = (currentB>127) ? 255 : 0;
            image->donneesRGB[i] = image->donneesRGB[i+1]
                                 = image->donneesRGB[i+2]
                                 = (currentB>127) ? 255 : 0; // temp : gris -> n&b
        }

        trouve = 0;
        for (j=0; j<colonneEcriture && !trouve; j++) {
            if (tabCouleurs[j].c.r == currentR
                    && tabCouleurs[j].c.g == currentG
                    && tabCouleurs[j].c.b == currentB) { // Couleur existante dans le tableau de freq
                trouve = 1;
                tabCouleurs[j].freq++;
            }
        }
        if (!trouve) { // si la Couleur n'existe pas dans le tableau de freq
            tabCouleurs[colonneEcriture].c.r = currentR;
            tabCouleurs[colonneEcriture].c.g = currentG;
            tabCouleurs[colonneEcriture].c.b = currentB;
            tabCouleurs[colonneEcriture].freq = 1;
            colonneEcriture++;

            if (colonneEcriture == tailleTabCouleurs) {
                tailleTabCouleurs += 100;
                tabCouleurs = realloc(tabCouleurs, tailleTabCouleurs*sizeof(colorCount));
                if (!tabCouleurs) {
                    printf("Impossible de realloc tabCouleurs. Fermeture du programme !\n");
                    exit(-1);
                }
            }
        }
    }

    //printf("Il y a %d Couleurs dans l'image\n", colonneEcriture);

    int indice = indiceFreqMax(tabCouleurs, colonneEcriture);
    //int max = tabCouleurs[indice].freq;
    fond->r = tabCouleurs[indice].c.r;
    fond->g = tabCouleurs[indice].c.g;
    fond->b = tabCouleurs[indice].c.b;
    //printf("Couleur du fond : (%d,%d,%d) (apparait sur %d pixels)\n", fond->r, fond->g, fond->b, max);

    indice = indiceFreqSousMax(tabCouleurs, colonneEcriture, indice);
    //int sousmax = tabCouleurs[indice].freq;
    texte->r = tabCouleurs[indice].c.r;
    texte->g = tabCouleurs[indice].c.g;
    texte->b = tabCouleurs[indice].c.b;
    //printf("Couleur du texte : (%d,%d,%d) (apparait sur %d pixels)\n", texte->r, texte->g, texte->b, sousmax);
}

DonneesImageRGB* cropImageToRectangle(DonneesImageRGB* image, Rectangle rect) { // crops an image to its min bounding rectangle
    int newWidth = rect.w+1;
    int newHeight = rect.h;

    //printf("w = %d, h = %d\n", newWidth, newHeight);

    DonneesImageRGB* croppedImg = (DonneesImageRGB*)calloc(1,sizeof(DonneesImageRGB));
    if (!croppedImg) {
        printf("Impossible d'alloc croppedImg. Fermeture du programme !\n");
        exit(-1);
    }
    croppedImg->largeurImage = newWidth;
    croppedImg->hauteurImage = newHeight;
    croppedImg->donneesRGB = (unsigned char *)calloc(3*newWidth*newHeight,1);
    if (!croppedImg->donneesRGB) {
        printf("Impossible d'alloc croppedImg->donneesRGB. Fermeture du programme !\n");
        exit(-1);
    }

    int x,y;
    long long int i,index=0;

    for (y=rect.y1; y<rect.y2; y++) {
        for (x=rect.x1; x<=rect.x2; x++) {
            i = getIfromXYinImage(image,x,y);
            if (i<0) i=0; // because out of bounds at first for the end. it's normal.
            croppedImg->donneesRGB[index] = image->donneesRGB[i];
            croppedImg->donneesRGB[index+1] = image->donneesRGB[i+1];
            croppedImg->donneesRGB[index+2] = image->donneesRGB[i+2];
            index += 3;
        }
    }

    return croppedImg;
}

int detectLettersAndWords(DonneesImageRGB* image, Couleur couleurTexte, Mot** results, int* nbrDeMots, int* espaces) {
    int numberOfWords = 1;

    int width = image->largeurImage;
    int height = image->hauteurImage;
    	
	int err = 0;
    int i,x,y;
    int stopVertical = 0;
    int compteur = 0;
    int compteurLettres = 1; // (see 3 lines below)
    int noMore = 0;
	int espacementMots = .26*height; // minimum, based on statistics.
	
    results[0]->nbrLettres++; // because first letter
    results[0]->lettres[0].x = 0; // starts at 0 obviously.
    
    for (x=0; x<width; x++) {
        stopVertical = 0;
        for (y=height; y>=0.1*height && !stopVertical; y--) {
            if (CouleursEgales(getColorAt(image, x, y), couleurTexte)) {
                stopVertical = 1;
                compteur = 0;
                noMore = 0;
            }
        }
        if (!stopVertical) { // ligne verticale fond => find de lettre(/mot)
            compteur++;
            if (!noMore) {
                results[numberOfWords-1]->nbrLettres++;
                results[numberOfWords-1]->lettres[compteurLettres].x = x;
                compteurLettres++;
            }
            noMore = 1;
            if (compteur > espacementMots) {
                results[numberOfWords-1]->x = x;
                numberOfWords++;
                compteurLettres = 0;
                compteur = 0;
            }
        }
    }
    
    for (i=0; i<numberOfWords; i++) {
        if (results[i]->nbrLettres == 0) {
            results[i]->nbrLettres++;
            results[i]->lettres[0].x = results[i]->x;
            results[i]->lettres[0].carac = ' ';
        }
    }

    *nbrDeMots = numberOfWords;

    //printf("***debug*** Number of detected words : %d \n", numberOfWords);
    for(i=0; i<numberOfWords; i++) {
        espaces[i] = ((i>0) ? espaces[i-1] : 0) + results[i]->nbrLettres-(i==0 ? 1 : 0);
        espaces[i] += (i==(numberOfWords-1) ? 1 : 0);
        //printf("***debug***    Word #%d : %d letters\n", i, results[i]->nbrLettres-(i==0 ? 1 : 0)); // first letter thing
        //printf("***debug***    espaces[%d] == %d \n", i, espaces[i]);
        
        if ((results[i]->nbrLettres > 100) || (results[i]->nbrLettres < 0)) err = -1;
    }
    
    return err;
}

DonneesImageRGB* resizedCopy(DonneesImageRGB* image, int newWidth, int newHeight) {
    int w1=image->largeurImage;
    int h1=image->hauteurImage;

    double scaleWidth =  (double)newWidth / (double)w1;
    double scaleHeight = (double)newHeight / (double)h1;

    DonneesImageRGB* resizedImg = (DonneesImageRGB*)calloc(1,sizeof(DonneesImageRGB));
    if (!resizedImg) {
        printf("Impossible d'alloc resizedImg. Fermeture du programme !\n");
        exit(-1);
    }
    resizedImg->largeurImage = newWidth;
    resizedImg->hauteurImage = newHeight;
    resizedImg->donneesRGB = (unsigned char*)calloc(3*newWidth*newHeight,1);
    if (!resizedImg->donneesRGB) {
        printf("Impossible d'alloc resizedImg->donneesRGB. Fermeture du programme !\n");
        exit(-1);
    }

    int cx, cy, pixel, nearestMatch;

    for(cy = 0; cy < newHeight; cy++) {
        for(cx = 0; cx < newWidth; cx++) {
            pixel = 3*(cy*newWidth+cx);
            nearestMatch = ((int)(cy / scaleHeight) * (w1*3)) + ((int)(cx / scaleWidth) *3);
            resizedImg->donneesRGB[pixel] =  image->donneesRGB[nearestMatch];
            resizedImg->donneesRGB[pixel+1] =  image->donneesRGB[nearestMatch+1];
            resizedImg->donneesRGB[pixel+2] =  image->donneesRGB[nearestMatch+2];
        }
    }

    return resizedImg;
}


Mot** newTabMots(void) {
    Mot** tabMots = (Mot**)calloc(50,sizeof(Mot*));
    int i;
    for (i=0; i<50; i++) {
        tabMots[i] = (Mot*)calloc(1,sizeof(Mot));
        if (!tabMots[i]) {
            printf("Impossible d'alloc tabMots[i]. Fermeture du programme !\n");
            exit(-1);
        }
        tabMots[i]->lettres = (Lettre*)calloc(100,sizeof(Lettre));
        if (!tabMots[i]->lettres) {
            printf("Impossible d'alloc tabMots[i]->lettres. Fermeture du programme !\n");
            exit(-1);
        }
    }
    return tabMots;
}

Lettre* separateLetters(DonneesImageRGB* image, Couleur couleurTexte, Couleur couleurFond, Mot** results, int* nbrDeLettres) {
    int i,j,k;
    int nbrLettres=0, nbrMots=0;
//    int bestAngle;
    Rectangle cropRect;
    DonneesImageRGB* tmpImg;
    Rectangle minRect;
    
    if (0) printf("%d %d %d", couleurFond.r, couleurFond.g, couleurFond.b);
    
    for(i=0; i<20; i++) {
        if (results[i]->lettres[0].x > ((i>0) ? 0 : -1)) {
            nbrMots++;
            nbrLettres += results[i]->nbrLettres;
        }
    }
    *nbrDeLettres = nbrLettres;

    Lettre* tabLettres = (Lettre*)calloc(nbrLettres,sizeof(Lettre));
    if (!tabLettres) {
        printf("Impossible d'alloc tabLettres. Fermeture du programme !\n");
        exit(-1);
    }

    i=0;
    for(j=0; j<nbrMots; j++)
        if (results[j]->lettres[0].x > ((j>0) ? 0 : -1))
            for(k=0; k<results[j]->nbrLettres; k++) {
                tabLettres[i] = results[j]->lettres[k];
                i++;
            }
                        
    for(i=0; i<nbrLettres; i++) {
        int x1 = tabLettres[i].x;
        int x2 = (i==nbrLettres-1) ? image->largeurImage : tabLettres[i+1].x;
        cropRect = (Rectangle){ x1, 0, x2, image->hauteurImage, x2-x1, image->hauteurImage };
        tmpImg = cropImageToRectangle(image, cropRect);
        improveImage(tmpImg); // débruitage par filtre médian.
        minRect = getMinRect(tmpImg, couleurTexte);
        tmpImg = cropImageToRectangle(tmpImg, minRect);
        tabLettres[i].img = tmpImg;
        
/*      bestAngle = findBestAngle(tabLettres[i].img, couleurFond, couleurTexte);
        tmpImg = rotateImage(tmpImg, bestAngle, couleurFond);
        tabLettres[i].img = tmpImg;

        minRect = getMinRect(tabLettres[i].img, couleurTexte);
        tabLettres[i].img = cropImageToRectangle(tabLettres[i].img, minRect);
        printf("ptr img %d = %p\n", i, tabLettres[i].img);*/
    }

    return tabLettres;
}

imageMatricielle* getImageMatricielle(DonneesImageRGB* image, Couleur couleurTexte) {
    int w = image->largeurImage, h=image->hauteurImage;
    int i;

    imageMatricielle* newImg = (imageMatricielle*)calloc(1,sizeof(imageMatricielle));
    if (!newImg) {
        printf("Impossible d'alloc newImg. Fermeture du programme !\n");
        exit(-1);
    }
    newImg->w = w;
    newImg->h = h;
    newImg->data = (unsigned char*)calloc(w*h,1);
    if (!newImg->data) {
        printf("Impossible d'alloc newImg->data. Fermeture du programme !\n");
        exit(-1);
    }

    int index;
    for(i=0, index=0; i<3*w*h; i+=3,index++)
        //newImg->data[index] = (!(CouleursEgales(getColorAtI(image,i), couleurFond))) ? 1 : 0;
        newImg->data[index] = ((CouleursEgales(getColorAtI(image,i), couleurTexte))) ? 1 : 0;

    return newImg;
}

double compareLettres(imageMatricielle* imgSrc, imageMatricielle* imgPolice) {
    int w = imgSrc->w, h=imgSrc->h;
    if (w<1) w=1; // pour éviter les problèmes.
    if (h<1) h=1;

    float indiceDeRessemblance = 0.0f;
    unsigned int dataSize = w*h, i;

    for(i=0; i<dataSize; i++)
        indiceDeRessemblance += (double)( (imgSrc->data[i] == imgPolice->data[i]) ? 1.0f : 0.0f );

    return (double) (indiceDeRessemblance/(double)(dataSize)); // normalisation nbr de pixels
}

LettrePolice* initPoliceArial() {

    int compteurFichiers = 70;

    LettrePolice* lettresPolice = (LettrePolice*)calloc(compteurFichiers,sizeof(LettrePolice));
    if (!lettresPolice) {
        printf("Impossible d'alloc lettresPolice. Fermeture du programme !\n");
        exit(-1);
    }

    char tmpString[50];
    DonneesImageRGB* tmpImg = NULL;
    Rectangle minRect;
    Couleur noir = {0,0,0};

    FILE* db = fopen("./Arial/DB-Arial.txt", "r");

    int i, err = 0;
    for(i=0; i<compteurFichiers; i++) {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-result"  // shut up gcc (warning: ignoring return value of ‘scanf’, declared with attribute warn_unused_result [-Wunused-result])
        fscanf(db, "%s %c\n", lettresPolice[i].nomFichier, &lettresPolice[i].carac);    // a cause du -O3.   L'astuce de caster le return en void ne marche plus apparement.
#pragma GCC diagnostic pop
        //printf("--- %s %c ---\n", lettresPolice[i].nomFichier, lettresPolice[i].carac);
        strcpy(lettresPolice[i].nomPolice, "arial");
        strcpy(tmpString, "./Arial/");
        strcat(tmpString, lettresPolice[i].nomFichier);
        tmpImg = lisBMPRGB(tmpString);
        if (!tmpImg) { printf("Erreur lors du chargement du caractère '%c' ! \n", lettresPolice[i].carac); err = 1; }
        minRect = getMinRect(tmpImg, noir);
        lettresPolice[i].img = (!err) ? cropImageToRectangle(lisBMPRGB(tmpString), minRect) : NULL;
        lettresPolice[i].ratioWH = (!err) ? ((float)lettresPolice[i].img->largeurImage / (float)lettresPolice[i].img->hauteurImage) : 0;
    }

    fclose(db);

    free(tmpImg);

    return lettresPolice;
}

int detectNbrLines(DonneesImageRGB* image, int* tabY, Couleur couleurTexte) {
    int width = image->largeurImage;
    int height = image->hauteurImage;
    
    int i,j,k=0;
    int compteurLignes = 0;
    int stopHoriz = 0;
    
    for (j=0;j<height;j++) {
		for (i=0;i<width && !stopHoriz;i++) {
			if (CouleursEgales(getColorAt(image, i, j), couleurTexte)) { 
				stopHoriz = 1;
				if (compteurLignes > 10) { tabY[k] = j; k++; }
				compteurLignes = 0;
			}
			//printf("-----j=%d, i=%d, compteurLignes = %d\n", j, i, compteurLignes);
		}
		if (!stopHoriz) compteurLignes++;
		stopHoriz = 0;
	}
    //PRINT(k);
    //dispTab(tabY, k);
	return k+1; // because espacements, pas lignes
}

/**
 * \brief     Chaîne globale représentant le nom du fichier image .bmp à charger.
 */
char nomFichier[75];

/**
 * \brief     Ensemble des lettres de la police Arial. Allocation faite dans initPoliceArial(), appelé dans le main().
 */
LettrePolice* lettresArial = NULL;

int main(int argc, char **argv)
{
	setlocale(LC_ALL, ""); // support unicode ! (é, à, è etc.)
 
	printf("------------------------\n");
	printf("--------- µOCR ---------\n");
	printf("----------v2.7----------\n");
	printf("------(26/01/2013)------\n");
	printf("------------------------\n");
    
    int fileExists = 0;

    if (argc > 1) {
        strcpy(nomFichier, argv[1]);
        printf("Chargement de l'image : %s\n", nomFichier);
    } else {
        char input[50];
        input[0] = '\0';
        
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-result"  // shut up gcc (warning: ignoring return value of ‘scanf’, declared with attribute warn_unused_result [-Wunused-result])
												  // a cause du -O3.   L'astuce de caster le return en void ne marche pas(/plus?) apparement.
        while (strlen(input)<1) { printf("Veuillez taper le nom du fichier à charger : \n"); fgets(input, sizeof(input), stdin);; }			      
#pragma GCC diagnostic pop
        printf("\n");
        strcpy(nomFichier, input);
    }
    
    if (strEndsWith(nomFichier,".jpg") || strEndsWith(nomFichier,".gif") || strEndsWith(nomFichier,".png") || strEndsWith(nomFichier,".tif") || strEndsWith(nomFichier,".tiff")) {
			printf("Seules les images BMP (24 bit) sont supportées. Une conversion automatique va être tentée (via GraphicsMagick).\n");
			
			char cmd[100];
			sprintf(cmd, "convert %s %s.bmp", nomFichier, nomFichier);
			
			if (system(cmd) == 0) {
				printf("La conversion a réussie. Le nouveau fichier a été sauvé en tant que %s.bmp. Chargement de celui-ci.\n", nomFichier);
				strcat(nomFichier, ".bmp");
			} else {
				printf("La conversion a écouchée, veuillez convertir votre fichier manuellement et réessayer..\n");
				return -1;
			}
	} else if (!strEndsWith(nomFichier,".bmp")) {
		strcat(nomFichier, ".bmp");
	}
    
	FILE *in;
    if ((in = fopen(nomFichier, "r")))
    {
        fclose(in);
        fileExists = 1;
    }

    if (fileExists) {
		lettresArial = initPoliceArial();
		if (!lettresArial) { 
			printf("Problème d'initialisation de la police ! Veuillez vérifier que les fichiers de police sont bien présentset au bon endroit !\n");
			return -1;
		}
		initialiseGfx(argc, argv);
        prepareFenetreGraphique("micrOCR - Reconnaissance matricielle de caracteres", LargeurFenetre, HauteurFenetre);
        lanceBoucleEvenements();
    } else {
        printf("Erreur de l'ouverture du fichier '%s'. Veuillez vérifier son nom/existence. : \n", nomFichier);
        return -1;
    }

    return 0;
}

Rectangle checkManualRect(Rectangle manualRect, DonneesImageRGB* imageBest) {
	
	Rectangle rect = {manualRect.x1, manualRect.y1, manualRect.x2, manualRect.y2, manualRect.w, manualRect.h}; //copie
	
	Rectangle manualRectImg = { rect.x1 - ((largeurFenetre()-imageBest->largeurImage)/2),
								rect.y1 - ((hauteurFenetre()-imageBest->hauteurImage)/2),
								rect.x2 - ((largeurFenetre()-imageBest->largeurImage)/2),
								rect.y2 - ((hauteurFenetre()-imageBest->hauteurImage)/2),
								rect.w,
								rect.h  };
	
	//printf("manualRectImg : x1:%d, y1:%d, x2:%d, y2:%d, w:%d\n", manualRectImg.x1, manualRectImg.y1, manualRectImg.x2, manualRectImg.y2, manualRectImg.w);    
	
	if (manualRectImg.x1 < 0 || manualRectImg.y1 < 0 || manualRectImg.x2 < 0 || manualRectImg.y2 < 0
		|| manualRectImg.x1 > imageBest->largeurImage || manualRectImg.y1 > imageBest->hauteurImage
		|| manualRectImg.x2 > imageBest->largeurImage || manualRectImg.y2 > imageBest->hauteurImage ) {
			rect.h = -1;
	} else {
		rect.h = abs(rect.y2-rect.y1);
	}
	
	return rect;
}

int clicInRect(int x, int y, Rectangle rect) {
	return (x>=rect.x1 && x<=rect.x2 && y>=rect.y1 && y<=rect.y2); // on sait implicitement que x1<x2 et y1<y2
}

// free stuff of [memory]expensive stuff
void freeImageRGB(DonneesImageRGB* img) {
    if (img) {
        if (img->donneesRGB) {
            free(img->donneesRGB);
            img->donneesRGB = NULL;
        }
        free(img);
        img = NULL;
    }
}
void freeImageMat(imageMatricielle* imgMat) {
    if (imgMat) {
        if (imgMat->data) {
            free(imgMat->data);
            imgMat->data = NULL;
        }
        free(imgMat);
        imgMat = NULL;
    }
}
void freeLettre(Lettre* ltr) {
    if (ltr) {
        if (ltr->img) {
            //freeImageRGB(ltr->img); // inutile
        }
        if (ltr->imgMat) {
            freeImageMat(ltr->imgMat);
        }
        free(ltr);
        ltr = NULL;
    }
}
void freeMot(Mot* mot) {
    if (mot) {
        if (mot->lettres) {
            freeLettre(mot->lettres);
        }
        free(mot);
        mot = NULL;
    }
}
void freeLP(LettrePolice* lp) {
    if (lp) {
		if (lp->img) {
            freeImageRGB(lp->img);
        }
        if (lp->imgMat) {
            freeImageMat(lp->imgMat);
        }
        lp = NULL;
    }
}
void freeCorr(Correspondance* corr) {
    if (corr) {
        if (corr->lettre) {
            freeLettre(corr->lettre);
        }
        free(corr);
        corr = NULL;
    }
}

void paintBouton(Bouton b) {
	if (!b.isHidden) {
		epaisseurDeTrait(3);
		
		int xOff, yOff ;
		couleurCourante(60, 167, 181);
		
		if (b.isActive) {
			xOff = 2; yOff = -2;
		} else {
			xOff = 0; yOff = 0;
		}
		rectangle(largeurFenetre()*b.x+1+xOff, hauteurFenetre()*b.y+1+yOff, largeurFenetre()*(b.x)+b.w-1+xOff, hauteurFenetre()*(b.y)+b.h-1+yOff);
		couleurCourante(40, 147, 161);
		if (b.isActive) epaisseurDeTrait(4);
		drawRect(largeurFenetre()*b.x+xOff, hauteurFenetre()*b.y+yOff, largeurFenetre()*(b.x)+b.w+xOff, hauteurFenetre()*(b.y)+b.h+yOff);
		
		epaisseurDeTrait(1);
		couleurCourante(255,255,255);
		afficheChaine(b.txt, b.h/2.25, largeurFenetre()*(b.x)+4+xOff, .5*(2*hauteurFenetre()*(b.y)+b.h)-4+yOff);
	}
}


/* La fonction de gestion des evenements, appelee automatiquement par le systeme
des qu'une evenement survient */
void gestionEvenement(EvenementGfx evenement)
{
    static bool pleinEcran = false; // Pour savoir si on est en mode plein ecran ou pas
    static int rotationManuelle = false;
    static int debugDisplay = false;
    static int analysisDone = 0;
    static DonneesImageRGB *imageOriginale = NULL; // image affichée au départ
    static DonneesImageRGB *imageTraitement = NULL;
    static DonneesImageRGB *imageBest = NULL;
    static DonneesImageRGB *imageManualOrig = NULL;
    static DonneesImageRGB *imageManual = NULL;
    static DonneesImageRGB *image = NULL; // image affichée
    static DonneesImageRGB** tabImages = NULL;
    static Rectangle minRect; //init
    //static Rectangle rect; //celui affiché
    static Rectangle manualRect; // manual crop	
    static int tmpAngle = 0;
    static int bestAngle = 0;
    static Couleur couleurFond;
    static Couleur couleurTexte;
    static Couleur couleurFondOrig;
    static Couleur couleurTexteOrig;
    //static Couleur blanc = {255, 255, 255};
    static Couleur noir = {0, 0, 0};
    static Couleur* ptrTexte = NULL;
    static Couleur* ptrFond = NULL;
    static Couleur* ptrTexteOrig = NULL;
    static Couleur* ptrFondOrig = NULL;
    static int nbrLines;
    static Mot** tabMots = NULL;
    static Lettre* tabLettres = NULL;
    static int nbrDeLettres;
    static int showWordsBounds = 1;
    static Correspondance* tabCorresp = NULL;
	static imageMatricielle* imageMatLettrePolice = NULL;
	static imageMatricielle* imageMatLettreExtraite = NULL;
	static double echelleLettres = .5;
	static char bigString[9999];
	static char* bigStringCorrige = NULL;
	static Bouton tabBoutons[5];
    int i,j;

    switch (evenement)
    {
    case Initialisation:
        imageTraitement = lisBMPRGB(nomFichier); // On est en BGR !
		imageManualOrig = lisBMPRGB(nomFichier); // copy.
		imageOriginale = lisBMPRGB(nomFichier); // copy
		/* Il aurait fallu faire des memcpy... */
		
		ptrFondOrig = (Couleur*)malloc(sizeof(Couleur));
		ptrTexteOrig = (Couleur*)malloc(sizeof(Couleur));
		getBackgroundAndTextColor(imageOriginale, ptrFondOrig, ptrTexteOrig);
		couleurFondOrig = *ptrFondOrig;
		couleurTexteOrig = *ptrTexteOrig;
				
		improveImage(imageTraitement);
		improveImage(imageManualOrig);
		
		imageManual = imageManualOrig;

		imageBest = imageTraitement;
		
		image = imageTraitement;
		
		manualRect.w = -1;
		
		tabBoutons[0] = (Bouton){0.03, 0.92, 120, 30, "Lancer [a]nalyse", 0, false, false};
		tabBoutons[1] = (Bouton){0.3, 0.92, 200, 30, "Passer en [m]ode manuel", 1, false, false};
		tabBoutons[2] = (Bouton){0.88, 0.92, 65, 30, "[R]eset", 2, false, false};
		tabBoutons[3] = (Bouton){0.88, 0.04, 65, 30, "[Q]uitter", 3, false, false};
		tabBoutons[4] = (Bouton){0.02, 0.08, 200, 30, "Verification orthographique", 4, false, false};

        //demandeAnimation_ips(5);
        redimensionneFenetre(LargeurFenetre, HauteurFenetre);
        break;

    case Affichage:
        effaceFenetre(230,230,230);

        if (image != NULL) {
						
            if (rotationManuelle) {
                if (!analysisDone)
					imageManual = ((tmpAngle%360)!=0) ? rotateImage(imageOriginale, tmpAngle%360, (Couleur){230,230,230}) : imageOriginale;
                image = imageManual;
            } else {
				if (!analysisDone) 
					image = imageOriginale;
				else
					image = imageBest;
                //rect = minRect;
            }

            ecrisImage((largeurFenetre()-image->largeurImage)/2, (hauteurFenetre()-image->hauteurImage)/2, image->largeurImage, image->hauteurImage, image->donneesRGB);
			
			if (image->largeurImage > largeurFenetre()) redimensionneFenetre(image->largeurImage, hauteurFenetre());
			if (image->hauteurImage > hauteurFenetre()) redimensionneFenetre(largeurFenetre(), image->hauteurImage);
			
            epaisseurDeTrait(1);
            // affichage du meilleur rectangle
            couleurCourante(255,0,0);
            
            if (analysisDone && !rotationManuelle) {
				drawRect( (largeurFenetre()-image->largeurImage)/2-1, // on triche en affichant directement (+rapide !) la bordure de l'image,
						  (hauteurFenetre()-image->hauteurImage)/2-1, // mais ca revient exactement au meme, puisqu'elle est rognée au minRect !
						  (largeurFenetre()+image->largeurImage)/2+1,
						  (hauteurFenetre()+image->hauteurImage)/2+1 );
			}
            
            if (rotationManuelle && !analysisDone) {
				if (manualRect.w > 0) {
					epaisseurDeTrait(1);
					couleurCourante(0,100,100);
					if (manualRect.h == -1) couleurCourante(255,0,0);
					drawRect(manualRect.x1, manualRect.y1, manualRect.x2, manualRect.y2);
				}
			}
			couleurCourante(0,0,0);
			if (analysisDone) {
				// coupure mots
				if (showWordsBounds) {
					epaisseurDeTrait(4);
					couleurCourante(0,0,255);
					for (i=0; i<20; i++)
						if (tabMots[i])
							ligne((largeurFenetre()-image->largeurImage)/2+tabMots[i]->x,
								  (hauteurFenetre()-image->hauteurImage)/2-30,
								  (largeurFenetre()-image->largeurImage)/2+tabMots[i]->x,
								  (hauteurFenetre()+image->hauteurImage)/2+30 );

					epaisseurDeTrait(2);
					couleurCourante(0,255,0);
					for (i=0; i<20; i++)
						if (tabMots[i])
							for (j=0; j<50; j++)
								if (tabMots[i]->lettres[j].x > 0)
									ligne((largeurFenetre()-image->largeurImage)/2+tabMots[i]->lettres[j].x,
										  (hauteurFenetre()-image->hauteurImage)/2-15,
										  (largeurFenetre()-image->largeurImage)/2+tabMots[i]->lettres[j].x,
										  (hauteurFenetre()+image->hauteurImage)/2+15 );
				}

				if (debugDisplay) {
					// lettres séparées
					int prevWidth = 10;
					for(i=0; i<nbrDeLettres; i++) {
						ecrisImage(5+prevWidth, 160, tabLettres[i].img->largeurImage, tabLettres[i].img->hauteurImage, tabLettres[i].img->donneesRGB);
						prevWidth += tabLettres[i].img->largeurImage + 8;
					}
				
					couleurCourante(0,0,0);
					epaisseurDeTrait(2);
					// guesses
					prevWidth = 10;
					DonneesImageRGB* tmpImg = NULL;
					for(i=0; i<nbrDeLettres; i++) {
						if (isprint(tabCorresp[i].lettrePolice->carac)) { // safe
							if (tabLettres[i].img && tabCorresp[i].lettrePolice->img) {
								tmpImg = resizedCopy(tabCorresp[i].lettrePolice->img, tabLettres[i].img->largeurImage, tabLettres[i].img->hauteurImage);
								ecrisImage(5+prevWidth, (hauteurFenetre()+image->hauteurImage)/2+30,
										   tabLettres[i].img->largeurImage,
										   tabLettres[i].img->hauteurImage,
										   tmpImg->donneesRGB );
							}
							//sprintf(carac, "%c", (tabCorresp[i].lettrePolice->carac));
							//afficheChaine(carac, 16, 20+prevWidth, 90);
						}
						prevWidth += tabLettres[i].img->largeurImage + 8;
					}
					free(tmpImg);
				}
			
				// textes
				epaisseurDeTrait(1);
				couleurCourante(0,0,0);
				char phrase[200]; // par precaution
				//unsigned long int surface = rect.w * rect.h;
				sprintf(phrase, "Angle de rotation: %d deg.", rotationManuelle ? tmpAngle%360 : bestAngle/*, rect.w, rect.h, surface*/);
				afficheChaine(phrase, 16, 5, hauteurFenetre()-20);
				sprintf(phrase, "Couleur du fond : (%d,%d,%d)", couleurFondOrig.r, couleurFondOrig.g, couleurFondOrig.b);
				afficheChaine(phrase, 16, 5, hauteurFenetre()-70);
				sprintf(phrase, "Couleur du texte : (%d,%d,%d)", couleurTexteOrig.r, couleurTexteOrig.g, couleurTexteOrig.b);
				afficheChaine(phrase, 16, 5, hauteurFenetre()-45);
				sprintf(phrase, "Echelle probable du texte : %.2f (par rapport a la police, Arial)", echelleLettres);
				afficheChaine(phrase, 16, 5, hauteurFenetre()-95);
				sprintf(phrase, "Texte reconnu : %s", bigStringCorrige);
				afficheChaine(phrase, 20, 5, hauteurFenetre()-145);
				
				sprintf(phrase, "Affichage des [s]eparations : %s.", showWordsBounds ? "Oui" : "Non");
				afficheChaine(phrase, 16, 5, 10);
							
				paintBouton(tabBoutons[4]); // correction
								
            } else {
				
				epaisseurDeTrait(1);
				char phrase[250]; // par precaution
				/*sprintf(phrase, "Appuyez sur [a] a tout moment pour lancer l'analyse de l'image.");
				afficheChaine(phrase, 16, 5, hauteurFenetre()-20);
				sprintf(phrase, "Appuyez sur [m] pour %s le mode manuel.", rotationManuelle ? "desactiver" : "activer");
				afficheChaine(phrase, 16, 5, hauteurFenetre()-50);*/
				
				for (i=0; i<2; i++)
					paintBouton(tabBoutons[i]);
				
				if (rotationManuelle) {
					couleurCourante(0,0,0);
					sprintf(phrase, "Mode manuel :");
					afficheChaine(phrase, 16, 5, 72);
					sprintf(phrase, "_________");
					afficheChaine(phrase, 16, 5, 68);
					sprintf(phrase, "Selection manuelle du rectangle de rognage : avec la souris (click&drag)");
					afficheChaine(phrase, 16, 5, 40);
					sprintf(phrase, "Rotation manuelle de l'image : fleches ou [+] / [-]. Angle courant = %d", tmpAngle%360);
					afficheChaine(phrase, 16, 5, 10);
				}
			}
			
			for (i=2; i<4; i++)
				paintBouton(tabBoutons[i]);
			
          }
        break;

    case Clavier:
        //printf("%c : ASCII %d\n", caractereClavier(), caractereClavier());

        switch (caractereClavier())
        {
		case 'A':
		
		analyse: //pour boutons.
		case 'a':
			
			
			printf("Analyse demandée (mode %s.)\n", rotationManuelle ? "manuel" : "automatique");   
			
				// multithreading a faire ?
				
				bigString[0] = '\0';
			
				ptrFond = (Couleur*)malloc(sizeof(Couleur));
				ptrTexte = (Couleur*)malloc(sizeof(Couleur));
				getBackgroundAndTextColor(imageTraitement, ptrFond, ptrTexte);
				couleurFond = *ptrFond;
				couleurTexte = *ptrTexte;
								
				imageBest = imageTraitement;

				if (rotationManuelle) { // mode manuel.
								
					bestAngle = tmpAngle;
					imageBest = rotateImage(imageBest, bestAngle, couleurFond);
					
					// changment de repere manualRect
					Rectangle manualRectImg = { manualRect.x1 - ((largeurFenetre()-imageBest->largeurImage)/2),
												manualRect.y1 - ((hauteurFenetre()-imageBest->hauteurImage)/2),
												manualRect.x2 - ((largeurFenetre()-imageBest->largeurImage)/2),
												manualRect.y2 - ((hauteurFenetre()-imageBest->hauteurImage)/2),
												manualRect.w,
												manualRect.h  };
					
					//printf(" %d %d %d %d %d %d\n", manualRect.x1, manualRect.y1, manualRect.x2, manualRect.y2, manualRect.w, manualRect.h);
					//printf(" %d %d %d %d %d %d\n", manualRectImg.x1, manualRectImg.y1, manualRectImg.x2, manualRectImg.y2, manualRectImg.w, manualRectImg.h);
					
					// crop manuel selon rectangle
					minRect = (manualRectImg.h>0) ? manualRectImg : getMinRect(imageBest, couleurTexte);
					imageBest = cropImageToRectangle(imageBest, minRect);
					
					// re crop best
					minRect = getMinRect(imageBest, couleurTexte);
					imageBest = cropImageToRectangle(imageBest, minRect);
					
				} else {
									
					minRect = getMinRect(imageTraitement, couleurTexte);
					imageBest = cropImageToRectangle(imageTraitement, minRect);

					bestAngle = findBestAngle(imageBest, couleurFond, couleurTexte);
					imageBest = rotateImage(imageBest, bestAngle, couleurFond);
				}
				
				minRect = getMinRect(imageBest, couleurTexte);
				imageBest = cropImageToRectangle(imageBest, minRect);
				minRect = getMinRect(imageBest, couleurTexte);

				if (rotationManuelle) imageManual = imageBest;
				
				image = imageBest;
				
				int tabY[100];
				tabY[0] = image->hauteurImage;
				nbrLines = detectNbrLines(image, tabY, couleurTexte) - 1 ; // espacements utilisés ici, d'où le "-1"
				//PRINT(nbrLines);
				int ligneCourante;
				tabImages = (DonneesImageRGB**)calloc((nbrLines+1), sizeof(DonneesImageRGB*));
				switch (nbrLines) {
					case 0:
						tabImages[0] = image;
						break;
					case 1:
						tabImages[0] = cropImageToRectangle(image, (Rectangle){0, tabY[0], image->largeurImage, image->hauteurImage, image->largeurImage, image->hauteurImage-tabY[0]});
						tabImages[1] = cropImageToRectangle(image, (Rectangle){0, 0, image->largeurImage, tabY[0], image->largeurImage, tabY[0]});
						break;
					case 2:
						tabImages[0] = cropImageToRectangle(image, (Rectangle){0, tabY[1], image->largeurImage, image->hauteurImage, image->largeurImage, image->hauteurImage-tabY[1]});
						tabImages[1] = cropImageToRectangle(image, (Rectangle){0, tabY[0], image->largeurImage, tabY[1], image->largeurImage, tabY[1]-tabY[0]});
						tabImages[2] = cropImageToRectangle(image, (Rectangle){0, 0, image->largeurImage, tabY[0], image->largeurImage, tabY[0]});
						break;
					default:
						tabImages[0] = cropImageToRectangle(image, (Rectangle){0, tabY[nbrLines-1], image->largeurImage, image->hauteurImage, image->largeurImage, image->hauteurImage-tabY[nbrLines-1]});
						for (i=1; i<nbrLines; i++)
							tabImages[i] = cropImageToRectangle(image, (Rectangle){0, tabY[(nbrLines-1)-i], image->largeurImage, tabY[(nbrLines-1)-i+1], image->largeurImage, tabY[(nbrLines-1)-i+1] - tabY[(nbrLines-1)-i]});
						tabImages[nbrLines] = cropImageToRectangle(image, (Rectangle){0, 0, image->largeurImage, tabY[0], image->largeurImage, tabY[0]});
						break;
				}
				
				int tmpIndex = (nbrLines>0) ? nbrLines : 0; // wtf I can't inline the condition.....
				
				for (ligneCourante=0; ligneCourante <= tmpIndex ; ligneCourante++) {
									
					tabMots = newTabMots();
					int nbrDeMots;
					int* espaces = calloc(50,sizeof(int));
					
					int ok = detectLettersAndWords(tabImages[ligneCourante], couleurTexte, tabMots, &nbrDeMots, espaces);
					

					if (ok == -1) {
						printf("Erreur fatale lors de la détection des lettres. Fermeture du programme !\n");
						exit(-1);
					}
						
					tabLettres = separateLetters(tabImages[ligneCourante], couleurTexte, couleurFond, tabMots, &nbrDeLettres);

					int tmp,tmp2;
					
					for(tmp=0; tmp<nbrDeLettres; tmp++)
						tabLettres[tmp].imgMat = getImageMatricielle(tabLettres[tmp].img, couleurTexte);

					DonneesImageRGB* tmpResizedImg = NULL;
					tabCorresp = (Correspondance*)calloc(nbrDeLettres,sizeof(Correspondance));
					if (!tabCorresp) {
						printf("Impossible d'alloc tabCorresp. Fermeture du programme !\n");
						exit(-1);
					}
									
					Lettre lettreEmpty = { NULL, NULL, -1, ' ' };    // mettre un caractère non imprimable ?
					LettrePolice lettrePoliceEmpty = { NULL, NULL, 1, ' ', "./Arial/arial_ptinterro.bmp", "arial" };
					Correspondance correspEmpty = { &lettreEmpty, &lettrePoliceEmpty, 9999};
					float tmpFiab = 0.0f;
					float ratioPixelsUnLettrePolice = 0.0f;
					float ratioPixelsUnLettreExtraite = 0.0f;
					float ratioWHLettrePolice;
					int tmpRatioCount = 0;
					int tmp3;
					float ratio;
					float* tabNotes = (float*)calloc(70,sizeof(float));
					float tmpNote, lambda1=10, lambda2=3, lambda3=19;
					
					for(tmp=0; tmp<nbrDeLettres; tmp++) {
						//printf("------lettre n°%d---\n", tmp+1);
						
						tabCorresp[tmp] = correspEmpty;
						
						imageMatLettreExtraite = tabLettres[tmp].imgMat;
						
						for (tmp3=0; tmp3<(imageMatLettreExtraite->h*imageMatLettreExtraite->w); tmp3++)
							ratioPixelsUnLettreExtraite += imageMatLettreExtraite->data[tmp3];
						ratioPixelsUnLettreExtraite /= (imageMatLettreExtraite->h*imageMatLettreExtraite->w); // le ratio
						
						ratio = (float)tabLettres[tmp].img->largeurImage / (float)tabLettres[tmp].img->hauteurImage;  // ratioWH
						
						tmpFiab = 9999.9;
						
						for (tmp2=0; tmp2<70; tmp2++) { // 70 = nombre de fichiers de la police Arial
						
							if (lettresArial[tmp2].ratioWH == 0) break;
							
							tmpRatioCount++;
							
							tmpResizedImg = resizedCopy(lettresArial[tmp2].img, tabLettres[tmp].img->largeurImage, tabLettres[tmp].img->hauteurImage);
							imageMatLettrePolice = getImageMatricielle(tmpResizedImg, noir);
							
							for (tmp3=0; tmp3<(imageMatLettrePolice->h*imageMatLettrePolice->w); tmp3++)
								ratioPixelsUnLettrePolice += imageMatLettrePolice->data[tmp3]; // nombre de un
							ratioPixelsUnLettrePolice /= (imageMatLettrePolice->h*imageMatLettrePolice->w); // le ratio.
							
							ratioWHLettrePolice = (float)imageMatLettrePolice->w / (float)imageMatLettrePolice->h;
							
							tmpNote = lambda1*(fabs(ratio-ratioWHLettrePolice) / ratio);
							tmpNote += lambda2*(fabs(ratioPixelsUnLettrePolice-ratioPixelsUnLettreExtraite) / ratioPixelsUnLettreExtraite);
														
							tabNotes[tmp2] = tmpNote;
							
							tabNotes[tmp2] += lambda3*(1.0-compareLettres(imageMatLettreExtraite, imageMatLettrePolice));
							
							tabNotes[tmp2] /= (lambda1+lambda2+lambda3);
							
							//printf(" tabNotes[tmp2] = %f  (%c) \n", tabNotes[tmp2], lettresArial[tmp2].carac);
							
							if (tabNotes[tmp2] < 0.1) { echelleLettres = ((double)tabLettres[tmp].img->largeurImage / (double)lettresArial[tmp2].img->largeurImage); }
							if (echelleLettres>.98) echelleLettres = 1;
							
							if (tabNotes[tmp2] <= tmpFiab) { // lettre trouvée mieux
								tabCorresp[tmp].fiabilite = tabNotes[tmp2];
								tabCorresp[tmp].lettre = &tabLettres[tmp];
								tabCorresp[tmp].lettrePolice = &lettresArial[tmp2];
								tmpFiab = tabCorresp[tmp].fiabilite;
							}
							
							ratioPixelsUnLettrePolice = 0;
							
						}
															
						tmpRatioCount = 0;
						ratioPixelsUnLettreExtraite = 0;
					}
					
					//printf("Echelle probable des lettres : %.3f (par rapport à la police)\n", echelleLettres);

					char* stringFinal = (char*)calloc(nbrDeLettres,1);
					for(tmp=0; tmp<nbrDeLettres; tmp++)
						stringFinal[tmp] = (tabCorresp[tmp].lettrePolice) ? tabCorresp[tmp].lettrePolice->carac : ' ';

					char* stringFinalAvecEspaces = (char*)calloc(nbrDeLettres+nbrDeMots+2,1);
					tmp = 0;
					for(i=0,j=0; i<(nbrDeLettres+nbrDeMots); i++,j++) {
						stringFinalAvecEspaces[i] = stringFinal[j];
						if (i==espaces[tmp]-1+tmp) {
							stringFinalAvecEspaces[i+1] = ' ';
							tmp++;
							i++;
						}
					}
					
					printf("Texte trouvé : %s\n", stringFinalAvecEspaces);
					
					strcat(stringFinalAvecEspaces, "\n");
					strcat(bigString, stringFinalAvecEspaces);
					bigStringCorrige = strdup(bigString);
					free(stringFinalAvecEspaces);
				}

				goto afterCorrect;
				
				correct:
					printf("Texte final non corrigé : %s\n", bigString);
					bigStringCorrige = spellCheck(bigString);
					printf("Texte final corrigé : %s\n", bigStringCorrige);
				
				afterCorrect:
					if (0) printf("42."); // wtf label error
					FILE * pFile;
					char* nomFichierOut = malloc(80*sizeof(char));
					strcpy(nomFichierOut, nomFichier);
					pFile = fopen (strcat(nomFichierOut,".txt"),"w");
					fputs(bigStringCorrige,pFile);
					fputs("\n ",pFile);
					fclose(pFile);
									
					printf("Texte final sauvegardé dans le fichier texte '%s'\n", nomFichierOut);
					free(nomFichierOut);
					
					//tmpAngle = 0;
					analysisDone = 1;
				
            rafraichisFenetre();
			break;
		
			
        case 'Q': /* Pour sortir quelque peu proprement du programme */
        case 'q':
			quit:
            // Free tous les trucs
            //printf("\n-------- Freeing memory --------\n");
            free(ptrFondOrig);
            free(ptrTexteOrig);
            freeImageRGB(imageTraitement);
            //printf("- freed imageTraitement\n");
            freeImageRGB(imageOriginale);
            //printf("- freed imageOriginale\n");
			freeImageMat(imageMatLettrePolice);
			//printf("- freed imageMatLettrePolice\n");
			freeImageMat(imageMatLettreExtraite);
			//printf("- freed imageMatLettreExtraite\n");
			free(ptrFond);
			//printf("- freed ptrFond\n");
			free(ptrTexte);
			//printf("- freed ptrTexte\n");
			for(i=0; i<70; i++)
				if (&lettresArial[i])
					freeLP(&lettresArial[i]);
			free(lettresArial);
			//printf("- freed lettresArial\n");
			if (analysisDone) {
				freeImageRGB(image);
				if (nbrLines>0)
					for (i=1; i<=nbrLines; i++)
						freeImageRGB(tabImages[i]);
				//printf("- freed images\n");
				freeCorr(tabCorresp);
				//printf("- freed tabCorresp\n");
				for(i=0; i<50; i++)
					if (tabMots[i])
						freeMot(tabMots[i]);
			}
			if (bigStringCorrige) {
				free(bigStringCorrige);
				//printf("- freed bigStringCorrige\n");
			}
			if (tabMots) free(tabMots);
				//printf("- freed tabMots\n");
			//printf("------- Free() done -------\n");
            
            exit(0);
            break;
            
        case 'D':
        case 'd':
			debugDisplay = !debugDisplay;
			rafraichisFenetre();
			break;

        case 'F':
        case 'f':
            pleinEcran = !pleinEcran; // Changement de mode plein ecran
            if (pleinEcran)
                modePleinEcran();
            else
                redimensionneFenetre(image ? image->largeurImage : LargeurFenetre, image ? image->hauteurImage : HauteurFenetre);
			rafraichisFenetre();
            break;

        case 'M':
        case 'm':
			switchMan:
			analysisDone = 0;
			imageBest = imageTraitement;
			rotationManuelle = !rotationManuelle;
			if (strcmp(tabBoutons[1].txt, "Passer en [m]ode manuel")==0) {
				strcpy(tabBoutons[1].txt, "Passer en [m]ode automatique");
				tabBoutons[1].w += 30;
			} else {
				strcpy(tabBoutons[1].txt, "Passer en [m]ode manuel");
				tabBoutons[1].w -= 30;
			}
			
			rafraichisFenetre();
            break;
            
        case 'R':
        case 'r':
			reset:
			tmpAngle = 0;
			analysisDone = 0;
			manualRect.w = -1;
			imageBest = imageTraitement;
			rafraichisFenetre();
			break;

		anglePlus:
        case '+':
            tmpAngle += rotationManuelle ? 1 : 0;
            //if (image != imageOriginale) { freeImageRGB(image); } // probleme (crash) si appelé à tres petit intervalle de temps... (touche restée appuyée)
			analysisDone = 0;
            rafraichisFenetre();
            break;

		angleMoins:
        case '-':
            tmpAngle -= rotationManuelle ? 1 : 0;
            //if (image != imageOriginale) { freeImageRGB(image); } // probleme (crash) si appelé à tres petit intervalle de temps... (touche restée appuyée)
			analysisDone = 0;
            rafraichisFenetre();
            break;

        case 'S':
        case 's':
            //affichage de la séparation des mots/lettres ou non
            showWordsBounds = !showWordsBounds;
            rafraichisFenetre();
            break;
        }
        break;

    case ClavierSpecial:
    
		switch( toucheClavier() ) {
			case 13:
			case 16:
				goto anglePlus;
				break;
				
			case 14:
			case 15:
				goto angleMoins;
				break;
		}
			
        break;

    case BoutonSouris:

			if (etatBoutonSouris() == GaucheAppuye)
			{	
				int stop = 0;
				for (i=0; i<nbrBoutons; i++) {
					if (clicInRect(abscisseSouris(), ordonneeSouris(), (Rectangle){largeurFenetre()*tabBoutons[i].x,
																			   hauteurFenetre()*tabBoutons[i].y,
																			   largeurFenetre()*tabBoutons[i].x+tabBoutons[i].w,
																			   hauteurFenetre()*tabBoutons[i].y+tabBoutons[i].h, 0, 0}) ) {
						tabBoutons[i].isActive = true;
						stop = 1;
						break;
					}
				}
				if (rotationManuelle && !analysisDone && !stop) {
					//printf("GaucheAppuye en : (%d, %d)\n", abscisseSouris(), ordonneeSouris());
					manualRect = (Rectangle){0,0,0,0,0,0};
					manualRect.x1 = abscisseSouris();
					manualRect.y1 = ordonneeSouris();
				}
			
			}
			else if (etatBoutonSouris() == GaucheRelache)
			{
				int stop = 0;
				for (i=0; i<nbrBoutons; i++) {
					if (clicInRect(abscisseSouris(), ordonneeSouris(), (Rectangle){largeurFenetre()*tabBoutons[i].x,
																				   hauteurFenetre()*tabBoutons[i].y,
																				   largeurFenetre()*tabBoutons[i].x+tabBoutons[i].w,
																				   hauteurFenetre()*tabBoutons[i].y+tabBoutons[i].h, 0, 0}) ) {
						if (tabBoutons[i].isActive) {
							tabBoutons[i].isActive = false;
							stop = 1;
							switch (tabBoutons[i].id) {
								case 0:
									goto analyse;
									break;
								case 1:
									goto switchMan;
									break;
								case 2:
									goto reset;
									break;
								case 3:
									goto quit;
									break;
								case 4:
									goto correct;
									break;
							}
						break;
						}
					}
					tabBoutons[i].isActive = false;
				}
				
				if (rotationManuelle && !analysisDone && !stop) {
					//printf("GaucheRelache en : (%d, %d)\n", abscisseSouris(), ordonneeSouris());
					manualRect.x2 = abscisseSouris();
					manualRect.y2 = ordonneeSouris();
					manualRect.w = abs(manualRect.x2-manualRect.x1);
					
					if (manualRect.x1 > manualRect.x2) SWAP(manualRect.x1, manualRect.x2);
					if (manualRect.y1 > manualRect.y2) SWAP(manualRect.y1, manualRect.y2);
				
					manualRect = checkManualRect(manualRect, imageBest);
					
					if (manualRect.h == -1) {
						//printf("Rectangle de rognage invalide, merci d'en resélectionner un.\n");
						manualRect.w = -1; // invalid (will not be drawn)
					}
				}
			}
			
			rafraichisFenetre();
			
        break;


    case Souris: // Si la souris est deplacee
        //printf("Souris déplacée en : (%d, %d)\n", abscisseSouris(), ordonneeSouris());
        if (rotationManuelle && !analysisDone) {
			manualRect.x2 = abscisseSouris();
			manualRect.y2 = ordonneeSouris();
			manualRect.w = abs(manualRect.x2-manualRect.x1);
			
			manualRect = checkManualRect(manualRect, imageBest);
			
			rafraichisFenetre();
		}
        break;

    case Inactivite: // Quand aucun message n'est disponible
        break;

    case Redimensionnement: // La taille de la fenetre a ete modifie ou on est passe en plein ecran
        // Donc le systeme nous en informe
        //printf("Largeur : %d\t", largeurFenetre());
        //printf("Hauteur : %d\n", hauteurFenetre());
        break;
    }
}

