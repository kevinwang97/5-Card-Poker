#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define OUTPUT_FILE "pokerHandResults.txt"
#define CHAR_LENGTH 20
//CARD HAND RANKING
#define NUM_HAND_TYPE 10
#define ROYAL_FLUSH 9
#define STRAIGHT_FLUSH 8
#define FOUR_OF_A_KIND 7
#define FULL_HOUSE 6
#define FLUSH 5
#define STRAIGHT 4
#define THREE_OF_A_KIND 3
#define TWO_PAIR 2
#define ONE_PAIR 1
#define BUST 0
//Game Constants
#define NUM_SUITS 4
#define NUM_RANKS 13
#define NUM_CARDS 52
#define HAND_SIZE 5
#define MAX_PLAYER 6

const char cardHandRanking [NUM_HAND_TYPE][CHAR_LENGTH] =
	{"Bust", "One pair", "Two pair", "Three of a kind", "Straight", "Flush",
	"Full house", "Four of a kind", "Straight Flush", "Royal Flush"};
typedef enum {two, three, four, five, six, seven, eight, nine, ten, jack, queen, king, ace} RankType;
const char rank[NUM_RANKS][CHAR_LENGTH] =
	{"two", "three", "four", "five", "six", "seven", "eight",
	"nine", "ten", "jack", "queen", "king", "ace"};
typedef enum {clubs, diamonds, hearts, spades} SuitType;
const char suit[NUM_SUITS][CHAR_LENGTH] = {"clubs", "diamonds", "hearts", "spades"};

typedef struct {
	SuitType suit;
	RankType rank;
} Card;

void initDeck (Card *deck) {
	for (int i = 0; i < NUM_SUITS; i++) {
		for (int j = 0; j < NUM_RANKS; j++) {
			deck[i * NUM_RANKS + j].suit = (SuitType) i;
			deck[i * NUM_RANKS + j].rank = (RankType) j;
		}
	}
}

void swap (Card *a, Card *b) {
	Card temp = *a;
	*a = *b;
	*b = temp;
}

//Fisher-Yates algorithm
void shuffleDeck (Card *deck) {
	for (int i = 0; i < NUM_CARDS; i++) {
		int randomIndex = rand () % (NUM_CARDS - i) + i;
		swap (&deck[randomIndex], &deck[i]);
	}
}

void dealHands (Card *deck, Card hand[][HAND_SIZE], int numPlayers) {
	for (int i = 0; i < HAND_SIZE; i++)
		for (int j = 0; j < numPlayers; j++)
			hand[j][i] = deck[i * numPlayers + j];
}

//returns 0 if no straight, otherwise returns lowest card in straight
int isStraight (int *cardValueCount) {
	int straightStarted = 0, startVal = 0, count = 0;
	for (int i = 0; i < NUM_RANKS; i++) {
		if (!straightStarted && cardValueCount[i] == 1) {
			straightStarted = 1;
			startVal = i + 2; //cards start at 2
			count++;
		}
		else if (straightStarted && count == HAND_SIZE - 1 && startVal == 2 && cardValueCount[12] == 1) return 1; // 'wheel' or 'bicycle' straight (A2345)
		else if (!straightStarted && cardValueCount[i] > 1 || straightStarted && cardValueCount[i] != 1) return 0;
		else if (straightStarted && cardValueCount[i] == 1) count++;
		if (count == HAND_SIZE) return startVal;
	}
}

int isFlush (Card *hand) {
	SuitType c = hand[0].suit;
	for (int i = 0; i < HAND_SIZE; i++) {
		if (hand[i].suit != c) return 0;
	}
	return 1;
}

void getNumNOfAKind (int *cardValueCount, int n, int *nOfAKind) {
	int num = 0;
	int ind = 0;
	for (int i = NUM_RANKS - 1; i >= 0; i--) {
		if (cardValueCount[i] == n) nOfAKind[ind++] = i + 2;
	}
}

void evaluateHand (Card *hand, int *cardRankCount, int* handScore) {
	int flush, straight;
	int quad[1] = {0}, triple[1] = {0}, pair[2] = {0}, single[HAND_SIZE] = {0};
	
	flush = isFlush (hand);
	straight = isStraight (cardRankCount);
	getNumNOfAKind (cardRankCount, 4, quad);
	getNumNOfAKind (cardRankCount, 3, triple);
	getNumNOfAKind (cardRankCount, 2, pair);
	getNumNOfAKind (cardRankCount, 1, single);
	if (straight == 10 && flush) {
		handScore[0] = ROYAL_FLUSH;
		return;
	}
	else if (straight != 0 && flush) {
		handScore[0] = STRAIGHT_FLUSH;
		handScore[1] = straight;
		return;
	} //{8, 9} represents 9,10,J,Q,K
	else if (quad[0] != 0) {
		handScore[0] = FOUR_OF_A_KIND;
		handScore[1] = quad[0];
		handScore[2] = single[0];
		return;
	} //{7, E, 2} represents A,A,A,A,2
	else if (triple[0] != 0 && pair[0] != 0) {
		handScore[0] = FULL_HOUSE;
		handScore[1] = triple[0];
		handScore[2] = pair[0];
		return;
	} //{6, E, D} represents A,A,A,K,K
	else if (flush) {
		handScore[0] = FLUSH;
		for (int i = 1; i < 1 + HAND_SIZE; i++) handScore[i] = single[i];
		return;
	} //{5, E, 6, 5, 4, 3} represents A,3,4,5,6
	else if (straight) {
		handScore[0] = STRAIGHT;
		handScore[1] = straight;
		return;
	} //{4, 2} represents 2,3,4,5,6
	else if (triple[0] != 0) {
		handScore[0] = THREE_OF_A_KIND;
		handScore[1] = triple[0];
		for (int i = 0; i < 2; i++) handScore[i + 2] = single[i];
		return;
	} //{3, E, 7, 6} represents A,A,A,7,6
	else if (pair[0] != 0 && pair[1] != 0) {
		handScore[0] = TWO_PAIR;
		for (int i = 0; i < 2; i++) handScore[i + 1] = pair[i];
		handScore[3] = single[0];
		return;
	} //{2, E, D, 6} represents A,A,K,K,6
	else if (pair[0] != 0) {
		handScore[0] = ONE_PAIR;
		handScore[1] = pair[0];
		for (int i = 0; i < 3; i++) handScore[i + 2] = single[i];
		return;
	} //{1, E, 6, 5, 4} represents A,A,4,5,6
	else {
		handScore[0] = BUST;
		for (int i = 0; i < HAND_SIZE; i++) handScore[i + 1] = single[i];
		return;
	} //{0, E, 6, 5, 4, 3} represents A,3,4,5,6
}

void evaluateWinner (Card hand[][HAND_SIZE], int numPlayers, int handScores[][6], int *bestHandIndices, int *numTied) {
	int cardRankCount[numPlayers][NUM_RANKS];
	memset(cardRankCount, 0, numPlayers * NUM_RANKS * sizeof(int));
	
	for (int i = 0; i < numPlayers; i++)
		for (int j = 0; j < HAND_SIZE; j++)
			cardRankCount[i][hand[i][j].rank]++;
	for (int i = 0; i < numPlayers; i++)
		evaluateHand (&hand[i][0], &cardRankCount[i][0], &handScores[i][0]);
	
	for (int i = 1; i < numPlayers; i++) {
		if (handScores[i][0] > handScores[bestHandIndices[0]][0]) {
			*numTied = 0;
			bestHandIndices[*numTied] = i;
		}
		else if (handScores[i][0] == handScores[bestHandIndices[0]][0]) {
			for (int j = 1; j < 1 + HAND_SIZE; j++) {
				if (handScores[i][j] > handScores[bestHandIndices[0]][j]) {
					bestHandIndices[*numTied] = i;
					break;
				}
				else if (j == HAND_SIZE - 1 && handScores[i][HAND_SIZE] == handScores[bestHandIndices[0]][HAND_SIZE]) {
					bestHandIndices[++*numTied] = i;
					break;
				}
				else break;
			}
		}
	}
}

void getGameInfo (int *numGames) {
	*numGames = 0;
	while (*numGames < 1 || *numGames > 20){
		printf ("How many games of widget? (between 1 and 20 inclusive)\n");
		scanf ("%i", numGames);
	}	
}

void getNumPlayers (int *numPlayers){
	*numPlayers = 0;
	while (*numPlayers < 2 || *numPlayers > 6){
		printf ("How many players? (between 2 and 6 inclusive)\n");
		scanf ("%i", numPlayers);
	}
}

void displayCards (Card *cards, int numCards) {
	for (int i = 0; i < numCards; i++)
		printf ("%i. %s of %s\n", i + 1, rank[cards[i].rank], suit[cards[i].suit]);
}

void printResults (Card *cards, int numCards, int player, int handScores[][6]){
	FILE *fp;
	fp = fopen (OUTPUT_FILE, "a+");
	fprintf (fp,"\nPlayer %i (%s)\n\n", player + 1, cardHandRanking [handScores[player][0]]);
	for (int i = 0; i < HAND_SIZE; i++)
		fprintf (fp,"%s %s\n", suit[cards[i].suit], rank[cards[i].rank]);
	fclose (fp);
}

void initFile (int numGames){
	FILE *fp; 
	fp = fopen (OUTPUT_FILE, "w");
	fprintf (fp,"%i Games of Widget will be played!\n===========================\n", numGames);
	fclose (fp);
}

void printfName (int game, int numPlayers){
	FILE *fp; 
	fp = fopen (OUTPUT_FILE, "a+");
	fprintf (fp,"Game #%i (%i players)\n", game, numPlayers);
	fclose (fp);
}

void printWinner (int game, Card hand[][HAND_SIZE], int numPlayers, int handScores[][6], int *bestHandIndices, int numTied){
	FILE *fp;
	fp = fopen (OUTPUT_FILE, "a+");
	if (numTied == 0)
		fprintf (fp,"\nThe Winner of game %i is", game, bestHandIndices[0] + 1, cardHandRanking[handScores[bestHandIndices[0]][0]]);
	else {
		fprintf (fp,"\nThe Winners of game %i are", game);
	}
	for (int i = 0; i < numTied + 1; i++) {
		fprintf (fp," player %i with a %s", bestHandIndices[i] + 1, cardHandRanking[handScores[bestHandIndices[i]][0]]);
	}	
	fprintf (fp,"\n==========================================================\n\n");
	fclose (fp);
}

int main () {
	srand (time (NULL));
	int numGames, numPlayers;
	Card deck[NUM_CARDS];
	Card hand[MAX_PLAYER][HAND_SIZE];
	getGameInfo (&numGames);
	initFile (numGames);
	for (int i = 0; i < numGames; i++) {
		getNumPlayers (&numPlayers);
		int handScores[numPlayers][1 + HAND_SIZE];
		memset(handScores, 0, numPlayers * (1 + HAND_SIZE) * sizeof(int));
		int bestHandIndices[numPlayers];
		memset (bestHandIndices, 0, numPlayers*sizeof(int));
		int *numTied = (int *)malloc(sizeof(int));
		*numTied = 0;
		printfName (i + 1, numPlayers);
		initDeck (deck);
		shuffleDeck (deck);
		dealHands (deck, hand, numPlayers);
		evaluateWinner (hand, numPlayers, handScores, bestHandIndices, numTied);
		for (int j = 0; j < numPlayers; j++) {
			printResults (&hand[j][0], HAND_SIZE, j, handScores);
		}
		printWinner (i + 1, hand, numPlayers, handScores, bestHandIndices, *numTied);
	}
}

