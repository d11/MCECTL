import Text.ParserCombinators.Parsec 
import IO hiding (try)
--import Prelude

data Formula = Conj Formula Formula
             | Disj Formula Formula
             | Neg Formula
             | PVar Char
             deriving (Show, Eq) 

pvar :: Parser Formula 
pvar = do c <- letter;
          return (PVar c)

negation :: Parser Formula 
negation = do char '!';
              f <- formula;
              return (Neg f)

conjunction :: Parser Formula 
conjunction = binary_op '&' Conj

disjunction :: Parser Formula 
disjunction = binary_op '|' Disj

binary_op :: Char -> (Formula -> Formula -> Formula) -> Parser Formula
binary_op symbol construct 
   = do char '(';
        f <- formula;
        char symbol;
        g <- formula;
        char ')';
        return (construct f g)

formula :: Parser Formula 
formula = try conjunction
      <|> disjunction
      <|> negation
      <|> pvar

simple :: Parser Char
simple = letter

run :: Show a => Parser a -> String -> IO ()
run p input
   = case (parse p "" input) of
        Left err -> do{ putStr "parse error at " ; print err }
        Right x -> print x


runLine :: String -> IO()
runLine line = run formula line

loopInput :: Handle -> IO()
loopInput handle = do putStrLn "Enter a CTL formula:";
                      stop <- hIsEOF handle;
                      if stop then do putStrLn "Goodbye!";
                                      return()
                              else do line <- hGetLine handle;
                                      runLine line;
                                      loopInput handle

main :: IO ()  
main = loopInput stdin
       
