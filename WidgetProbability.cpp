#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define NUM_TRIALS 600000
#define OUTPUT_FILE "widgetHandProbabilities.txt"
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
#define NUM_CARD_HAND 5
#define NUM_HANDS 12

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

void dealHands (Card *deck, Card hand[NUM_HANDS][NUM_CARD_HAND]) {
	for (int i = 0; i < NUM_CARD_HAND; i++)
		for (int j = 0; j < NUM_HANDS; j++)
			hand[j][i] = deck[i + j];
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
		if (count == NUM_CARD_HAND) return 1;
	}
}

int isFlush (Card *hand) {
	ColourType c = hand[0].colour;
	for (int i = 0; i < NUM_CARD_HAND; i++) {
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

int evaluateHand (Card *hand) {
	int cardValueCount[NUM_RANKS] = {0};
	int flush, straight, triple, numDouble;
	for (int i = 0; i < NUM_CARD_HAND; i++) {
		cardValueCount[hand[i].rank]++;
	}
	flush = isFlush (hand);
	straight = isStraight (cardValueCount);
	triple = numNOfAKind (cardValueCount, 3);
	numDouble = numNOfAKind (cardValueCount, 2);
	if (straight && flush) return STRAIGHT_FLUSH;
	else if (numNOfAKind (cardValueCount, 5) == 1) return FIVE_OF_A_KIND;
	else if (flush) return FLUSH;
	else if (numNOfAKind (cardValueCount, 4) == 1) return FOUR_OF_A_KIND;
	else if (triple == 1 && numDouble == 1) return FULL_HOUSE;
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
	Card hand[NUM_HANDS][NUM_CARD_HAND];
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
