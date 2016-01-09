#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define OUTPUT_FILE "widgetHandResults.txt"
//CARD HAND RANKING
#define NUM_HAND_TYPE 10
#define STRAIGHT_FLUSH 0
#define FIVE_OF_A_KIND 1
#define FLUSH 2
#define FOUR_OF_A_KIND 3
#define FULL_HOUSE 4
#define STRAIGHT 5
#define THREE_OF_A_KIND 6
#define TWO_PAIR 7
#define BUST 8
#define ONE_PAIR 9
//Poker Related Constants
#define NUM_COLOURS 7
#define NUM_RANKS 9
#define NUM_CARDS 63
#define HAND_SIZE 5
#define MAX_PLAYER 6

const char cardHandRanking [][20] = {"Straight Flush", "Five of a kind", "Flush", "Four of a kind", "Full house", "Straight",
	"Three of a kind", "Two pair", "Bust", "One pair"};
typedef enum RankType {one, two, three, four, five, six, seven, eight, nine};
const char rank[][10] {"one", "two", "three", "four", "five", "six", "seven", "eight", "nine"};
typedef enum ColourType {red, blue, green, yellow, black, pink, white};
const char colour[][10] = {"red", "blue", "green", "yellow", "black", "pink", "white"};

typedef struct {
	ColourType colour;
	RankType rank;
} Card;

void initDeck (Card *deck) {
	for (int i = 0; i < NUM_COLOURS; i++) {
		for (int j = 0; j < NUM_RANKS; j++) {
			deck[i * NUM_RANKS + j].colour = (ColourType) i;
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

int isStraight (int *cardValueCount) {
	int straightStarted = 0, count = 0;
	for (int i = 0; i < NUM_RANKS; i++) {
		if (!straightStarted && cardValueCount[i] == 1) {
			straightStarted = 1;
			count++;
		}
		else if (!straightStarted && cardValueCount[i] > 1 || straightStarted && cardValueCount[i] != 1) return 0;
		else if (straightStarted && cardValueCount[i] == 1) count++;
		if (count == HAND_SIZE) return 1;
	}
}

int isFlush (Card *hand) {
	ColourType c = hand[0].colour;
	for (int i = 0; i < HAND_SIZE; i++) {
		if (hand[i].colour != c) return 0;
	}
	return 1;
}

int numNOfAKind (int *cardValueCount, int n) {
	int num = 0;
	for (int i = 0; i < NUM_RANKS; i++) {
		if (cardValueCount[i] == n) num++;
	}
	return num;
}

int evaluateHand (Card *hand, int *cardRankCount) {
	int flush, straight, triple, numDouble;
	flush = isFlush (hand);
	straight = isStraight (cardRankCount);
	triple = numNOfAKind (cardRankCount, 3);
	numDouble = numNOfAKind (cardRankCount, 2);
	if (straight && flush) return STRAIGHT_FLUSH;
	else if (numNOfAKind (cardRankCount, 5) == 1) return FIVE_OF_A_KIND;
	else if (flush) return FLUSH;
	else if (numNOfAKind (cardRankCount, 4) == 1) return FOUR_OF_A_KIND;
	else if (triple == 1 && numDouble == 1) return FULL_HOUSE;
	else if (straight) return STRAIGHT;
	else if (triple == 1) return THREE_OF_A_KIND;
	else if (numDouble == 2) return TWO_PAIR;
	else if (numDouble == 1) return ONE_PAIR;
	else return BUST;
}

int evaluateTie (int handType, int *bestHandIndices, int numTied, int cardRankCount[][NUM_RANKS]) {
	unsigned int handScores[numTied + 1];
	unsigned int doubleMultiplier[numTied + 1];
	unsigned int singleMultiplier[numTied + 1];
	for (int i = 0; i < numTied + 1; i++) {
		handScores[i] = 0;
		doubleMultiplier[i] = 10000000; //double multiplier must be greater by x100 because there is a possibility of two pair
		singleMultiplier[i] = 100000;
	}
	for (int i = 0; i < numTied + 1; i++) {
		for (int j = NUM_RANKS - 1; j >= 0; j--) {
			if (cardRankCount[bestHandIndices[i]][j] == 5)
				handScores[i] += j; //this score can be j because the hand is just five-of-a-kind
			else if (cardRankCount[bestHandIndices[i]][j] == 4)
				handScores[i] += j * 1000000; //must be greater than singleton multiplier
			else if (cardRankCount[bestHandIndices[i]][j] == 3)
				handScores[i] += j * 100000000; //must be greater than double multiplier
			else if (cardRankCount[bestHandIndices[i]][j] == 2) {
				handScores[i] += j * doubleMultiplier[i];
				doubleMultiplier[i] /= 10;
			}
			else if (cardRankCount[bestHandIndices[i]][j] == 1) {
				handScores[i] += j * singleMultiplier[i];
				singleMultiplier[i] /= 10;
			}
		}
	}
	for (int i = 0; i < numTied + 1; i++) {
	//	printf ("Hand %i Score: %u\n", bestHandIndices[i], handScores[i]);
	}
	int bestHand = 0;
	for (int i = 1; i < numTied + 1; i++) {
		if (handScores[i] > handScores[bestHand]) bestHand = i;
		else if (handScores[i] == handScores[bestHand]) return -1; //TIE
	}
	return bestHandIndices[bestHand];
}

int evaluateWinner (Card hand[][HAND_SIZE], int numPlayers, int *handScores) {
	int cardRankCount[numPlayers][NUM_RANKS];
	//int handScores[numPlayers];
	int bestHandIndices[numPlayers];
	int numTied = 0;
	bestHandIndices[0] = 0;
	for (int i = 0; i < numPlayers; i++)
		for (int j = 0; j < NUM_RANKS; j++)
			cardRankCount[i][j] = 0;
	for (int i = 0; i < numPlayers; i++)
		for (int j = 0; j < HAND_SIZE; j++)
			cardRankCount[i][hand[i][j].rank]++;
	for (int i = 0; i < numPlayers; i++)
		handScores[i] = evaluateHand (&hand[i][0], &cardRankCount[i][0]);
	for (int i = 1; i < numPlayers; i++) {
		if (handScores[i] < handScores[bestHandIndices[0]]) {
			bestHandIndices[0] = i;
			numTied = 0;
		}
		else if (handScores[i] == handScores[bestHandIndices[0]])
			bestHandIndices[++numTied] = i;
	}
	for (int i = 0; i < numPlayers; i++) {
	//	printf ("Player %i has %s ", i + 1, cardHandRanking [handScores[i]]);
	//	printf ("\n");
	}
	if (numTied != 0) return evaluateTie (handScores[bestHandIndices[0]], bestHandIndices, numTied, cardRankCount);
	return bestHandIndices[0];
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
		printf ("%i. %s of %s\n", i + 1, rank[cards[i].rank], colour[cards[i].colour]);
}

void printResults (Card *cards, int numCards, int player, int *handScores){
	FILE *fp; 
	fp = fopen (OUTPUT_FILE, "a+");
	fprintf (fp,"\nPlayer %i (%s)\n\n", player+1, cardHandRanking [handScores[player]]);
	for (int i = 0; i < HAND_SIZE; i++)
		fprintf (fp,"%s %s\n", colour[cards[i].colour], rank[cards[i].rank]);
	fclose (fp);
}

void initFile (int numGames){
	FILE *fp; 
	fp = fopen (OUTPUT_FILE, "w");
	fprintf (fp,"%i Games of Widget will be played!\n===========================\n", numGames);
	fclose (fp);
}

void printfName (int game,int numPlayers){
	FILE *fp; 
	fp = fopen (OUTPUT_FILE, "a+");
	fprintf (fp,"Game #%i (%i players)\n", game,numPlayers);
	fclose (fp);
}

void printWinner (int game,Card hand[][HAND_SIZE], int numPlayers, int *handScores){
	int winner = evaluateWinner (hand, numPlayers,handScores);
	FILE *fp;
	fp = fopen (OUTPUT_FILE, "a+");
	fprintf (fp,"\nThe Winner of game %i is player %i with a %s\n",game,winner +1,cardHandRanking [handScores[winner]]);
	fprintf (fp,"==========================================================\n\n");
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
		int handScores[numPlayers];
		printfName (i+1,numPlayers);
		initDeck (deck);
		shuffleDeck (deck);
		dealHands (deck, hand, numPlayers);
		evaluateWinner (hand, numPlayers,handScores);
		for (int j = 0; j < numPlayers; j++) {
			printResults (&hand[j][0], HAND_SIZE, j, handScores);
		}
		printWinner (i+1,hand,numPlayers,handScores);
		//printf ("Winner is player %i\n", evaluateWinner (hand, numPlayers,handScores) + 1);
	}
}

