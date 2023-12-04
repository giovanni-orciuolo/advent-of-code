package main

import (
	"fmt"
	"regexp"
	"strconv"
	"strings"

	"github.com/giovanni-orciuolo/advent-of-code/aoc"
)

// checkRound returns true if the round is considered to be valid
func checkRound(round string) bool {
	re := regexp.MustCompile(`(?m)((\d+) ([red|green|blue]+))+`)
	for _, match := range re.FindAllStringSubmatch(round, -1) {
		color := match[3]
		amount, err := strconv.ParseInt(match[2], 10, 0)
		if err != nil {
			panic(err)
		}
		// fmt.Printf("Color: %s, Amount: %d\n", color, amount)

		switch color {
		case "red":
			if amount > 12 {
				return false
			}
		case "green":
			if amount > 13 {
				return false
			}
		case "blue":
			if amount > 14 {
				return false
			}
		}
	}

	return true
}

func maximumAmountsOfColorsInGame(game string) []int {
	rounds := strings.Split(game, ";")
	maximums := []int{-1, -1, -1} // Red, Green, Blue

	for _, round := range rounds {
		re := regexp.MustCompile(`(?m)((\d+) ([red|green|blue]+))+`)
		for _, match := range re.FindAllStringSubmatch(round, -1) {
			color := match[3]
			amount, err := strconv.ParseInt(match[2], 10, 0)
			if err != nil {
				panic(err)
			}
			
			switch color {
			case "red":
				if maximums[0] < int(amount) {
					maximums[0] = int(amount)
				}
			case "green":
				if maximums[1] < int(amount) {
					maximums[1] = int(amount)
				}
			case "blue":
				if maximums[2] < int(amount) {
					maximums[2] = int(amount)
				}
			}
		}
	}

	return maximums
}

func main() {
	validGamesIdSum := 0
	powerSum := 0

	for i, game := range aoc.ReadInput(2) {
		// Remove game id
		re := regexp.MustCompile(`(?m)Game \d: `)
		game = re.ReplaceAllString(game, "")

		// Split by ";" for different rounds
		rounds := strings.Split(game, ";")

		validGame := true
		for _, round := range rounds {
			// fmt.Println("Round: " + round)
			if !checkRound(round) {
				validGame = false
				break
			}
		}

		if validGame {
			validGamesIdSum += (i + 1)
		}

		maxColors := maximumAmountsOfColorsInGame(game)
		powerSum += (maxColors[0] * maxColors[1] * maxColors[2])
	}

	fmt.Printf("Part 1 solution: %d\n", validGamesIdSum)
	fmt.Printf("Part 2 solution: %d\n", powerSum)
}