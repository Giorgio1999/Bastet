#!/bin/sh

ENGINE1=$1
ENGINE2=$2
ELO0=$3
ELO1=$4

ALPHA="0.09"
BETA="0.09"
FILE="noob_2moves.epd"
FORMAT="epd"
TC="100/5+2"

cutechess-cli -engine conf=bastet-"${ENGINE1}" -engine conf=bastet-"${ENGINE2}" -each tc="${TC}" -sprt elo0="${ELO0}" elo1="${ELO1}" alpha="${ALPHA}" beta="${BETA}" -rounds 10000 -ratinginterval 10 -outcomeinterval 100 -openings file=test_suits/"${FILE}" format="${FORMAT}" order=random -repeat -games 2 -pgnout tournaments/"${ENGINE1}"vs"${ENGINE2}".pgn -recover
