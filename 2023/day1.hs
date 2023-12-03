{-# LANGUAGE LambdaCase #-}
module Main where

import Data.Char (digitToInt, isDigit)
import Data.List (tails)
import Data.Maybe (mapMaybe)

-- Helper to read input files
readFileLines :: FilePath -> IO [String]
readFileLines = fmap lines . readFile
--

firstDigit :: String -> Char
firstDigit str = head $ filter isDigit str

lastDigit :: String -> Char
lastDigit str = last $ filter isDigit str

combineFirstLastDigit :: String -> Int
combineFirstLastDigit str = read [firstDigit str, lastDigit str] :: Int

solveFirstPart :: [String] -> Int
solveFirstPart input = sum $ map combineFirstLastDigit input

parseDigit :: String -> Maybe Int
parseDigit = \case
  'z' : 'e' : 'r' : 'o' : _ -> Just 0
  'o' : 'n' : 'e' : _ -> Just 1
  't' : 'w' : 'o' : _ -> Just 2
  't' : 'h' : 'r' : 'e' : 'e' : _ -> Just 3
  'f' : 'o' : 'u' : 'r' : _ -> Just 4
  'f' : 'i' : 'v' : 'e' : _ -> Just 5
  's' : 'i' : 'x' : _ -> Just 6
  's' : 'e' : 'v' : 'e' : 'n' : _ -> Just 7
  'e' : 'i' : 'g' : 'h' : 't' : _ -> Just 8
  'n' : 'i' : 'n' : 'e' : _ -> Just 9
  x : _ | isDigit x -> Just $ digitToInt x
  _ -> Nothing

solveSecondPart :: [String] -> Int
solveSecondPart = sum . map adjust
  where
    firstDigit = head . mapMaybe parseDigit . tails
    lastDigit = head . mapMaybe parseDigit . reverse . tails
    adjust xs = 10 * firstDigit xs + lastDigit xs

main :: IO ()
main = do
    input <- readFileLines "inputs/day1.txt"
    print $ "First part solution: " ++ show (solveFirstPart input)
    print $ "Second part solution: " ++ show (solveSecondPart input)
