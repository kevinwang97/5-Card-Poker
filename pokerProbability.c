#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define NUM_TRIALS 600000
#define OUTPUT_FILE "pokerHandProbabilities.txt"
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
//Poker Related Constants
#define NUM_SUITS 4
#define NUM_RANKS 13
#define NUM_CARDS 52
#define HAND_SIZE 5
#define NUM_HANDS 12

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

void dealHands (Card *deck, Card hand[NUM_HANDS][HAND_SIZE]) {
	for (int i = 0; i < HAND_SIZE; i++)
		for (int j = 0; j < NUM_HANDS; j++)
			hand[j][i] = deck[i + j];
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

int numNOfAKind (int *cardValueCount, int n) {
	int num = 0;
	for (int i = 0; i < NUM_RANKS; i++) {
		if (cardValueCount[i] == n) num++;
	}
	return num;
}

int evaluateHand (Card *hand) {
	int cardRankCount[NUM_RANKS] = {0};
	int flush, straight, triple, quad, numDouble;
	for (int i = 0; i < HAND_SIZE; i++) {
		cardRankCount[hand[i].rank]++;
	}
	flush = isFlush (hand);
	straight = isStraight (cardRankCount);
	quad = numNOfAKind (cardRankCount, 4);
	triple = numNOfAKind (cardRankCount, 3);
	numDouble = numNOfAKind (cardRankCount, 2);
	
	if (straight == 10 && flush) return ROYAL_FLUSH;
	if (straight != 0 && flush) return STRAIGHT_FLUSH;
	else if (quad == 1) return FOUR_OF_A_KIND;
	else if (triple == 1 && numDouble == 1) return FULL_HOUSE;
	else if (flush) return FLUSH;
	else if (straight) return STRAIGHT;
	else if (triple == 1) return THREE_OF_A_KIND;
	else if (numDouble == 2) return TWO_PAIR;
	else if (numDouble == 1) return ONE_PAIR;
	else return BUST;
}

void printResults (int *results) {
	FILE *fp; 
	fp = fopen (OUTPUT_FILE, "w");
	fprintf (fp, "Name\t#Hands\tProbability\n");
	for (int i = 0; i < NUM_HAND_TYPE; i++)
		fprintf (fp, "%s %i %f\n", cardHandRanking[i], results[i], results[i]/(NUM_TRIALS * NUM_HANDS * 1.0));
	fclose (fp);
}

int main () {
	srand (time (NULL));
	Card deck[NUM_CARDS];
	Card hand[NUM_HANDS][HAND_SIZE];
	int handTypeCount [NUM_HAND_TYPE] = {0};
	
	initDeck (deck);
	for (int i = 0; i < NUM_TRIALS; i++) {
		shuffleDeck (deck);
		dealHands (deck, hand);
		for (int i = 0; i < NUM_HANDS; i++)
			handTypeCount[evaluateHand (&hand[i][0])]++;
	}
	printResults (handTypeCount);
	printf ("DONE");
}
