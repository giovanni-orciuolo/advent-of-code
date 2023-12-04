package aoc

import (
	"os"
	"strconv"
	"strings"
)

func ReadInput(day int) []string {
	content, err := os.ReadFile("inputs/day" + strconv.FormatInt(int64(day), 10) + ".txt")
	if err != nil {
		panic(err)
	}

	return strings.Split(string(content), "\n")
}