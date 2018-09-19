# -*- coding: utf-8 -*-
"""
Created on Tue Aug 21 08:52:58 2018

@author: pongor.csaba
"""
import os, sys, subprocess
#import warnings
import random, traceback
import numpy as np

SEPARATOR='#####################################################################################################################'

QUOTE_LIST=["You know you're a teacher when you say 2, write 3, and mean 4.\n     -Ronald Anstrom, high school teacher, Underwood, N.D. 1974",
		"Research is what I am doing when I don't know what I am doing.\n     -Werner von Braun",
		"Learn from yesterday, live for today, look to tomorrow, rest this afternoon.\n     -Snoopy",
		"The great thing about being imperfect is the joy it brings others.\n     -Sign outside Lake Agassiz Jr. High School, Fargo, N.D.",
		"It is unworthy of excellent men to lose hours like slaves in the labor of calculation which could be safely relegated to anyone else if a machine were used.\n     -G. W. von Leibniz",
		"Achievement - the man who rows the boat generally doesn't have time to rock it.\n     -From the back of a sugar packet",
		"Nothing will be attempted if all possible objections must first be overcome.\n     -The golden principle, Paul Dickson's ""The Official Rules""",
		"If you perceive that there are four possible ways in which a procedure can go wrong and circumvent these, then a fifth way will develop.\n     -Murphy's twelfth law",
		"Standing in the middle of the road is very dangerous; you get knocked down by the traffic from both sides.\n     -Margaret Thatcher",
		"You will never ""find"" time for anything. If you want time, you must make it.\n     -Charles Bixton)",
		"Be not the first by whom the new are tried, nor yet the the last to lay the old aside.\n     -Alexander Pope",
		"If it happens, it must be possible.\n     -The unnamed law from Paul Dickson's ""The Official Rules""",
		"Life can only be understood backward, but must be lived forward.\n     -Kirkegaard",
		"If you don't have the law - argue the facts. If you don't have the facts - argue the law. If you don't have either - pound the table.\n     -Gaussian QC",
		"You know you've spoken too long when the audience stops looking at their watches and starts shaking them.\n     -Gaussian QC",
		"The best way to pay for a lovely moment is to enjoy it.\n     -Richard Bach",
		"Happiness is not having what you want - happiness is wanting what you have!\n     -From Mrs. Severn's desk",
		"Ashes to ashes\nDust to dust\nOil those brains\nBefore they rust.\n     -J. Prelutsky",
		"The human brain, then, is the most complicated organization of matter that we know.\n     -Isaac Asimov",
		"The brain struggling to understand the brain is society trying to explain itself.\n     -Colin Blakemore",
		"Whatever happens in the mind of man is represented in the actions and interactions of brain cells.\n     -Geschwind and A.M. Galaburda",
		"Brain: an apparatus with which we think that we think.\n     -Ambrose Bierce)",
		"Mind: A mysterious form of matter secreted by the brain.\n     -Ambrose Bierce",
		"Ah! My poor brain is racked and crazed,\nMy spirit and senses amazed!\n     -Johann Wolfgang Von Goethe:Faust, 1808",
		"I am a brain, Watson. The rest of me is a mere appendix.\n     -Arthur Conan Doyle:Sherlock Holmes",
		"I like nonsense; it wakes up the brain cells.\n     -Dr. Seuss",
		"Money spent on brain is never spent in vain.\n     -English Proverb", 
		"Ask for advice, and then use your brain.\n     -Norwegian Proverb",
		"If you prick me, do I not leak\n     -Data from Star Trek",
       "Data: Apologies, Captain. I seem to have reached an odd functional impasse. I am stuck.\nPicard: Then get unstuck and continue with the briefing.\nData: Yes, sir. That is what I am trying to do, sir, but the solution eludes me.\n     -Star Trek:The Last Outpost",
		"Your focus determines your reality.\n     -Qui-Gon Jinn",
		"Do. Or do not. There is no try.\n     -Yoda",
		"Your eyes can deceive you. Don't trust them.\n     -Obi-Wan Kenobi",
		"A life is like a garden. Perfect moments can be had, but not preserved, except in memory.\n     -Leonard Nimoy, not Spock",
		"Scissors cuts paper, paper covers rock, rock crushes lizard, lizard poisons Spock,\nSpock smashes scissors, scissors decapitates lizard, lizard eats paper,\npaper disproves Spock, Spock vaporises rock, and as it always has, rock crushes scissors.\n     -Sheldon:The Big Bang Theory)",
		"Physics is like sex: sure, it may give some practical results, but thats not why we do it\n     -Richard P. Feynman",
		"You will have a long and wonderfull life\n     -Chinese fortune cookie",
		"If we knew what it was we were doing, it would not be called research, would it?\n     -Albert Einstein",
		"I have not failed. Ive just found 10,000 ways that wont work.\n     -Thomas Alva Edison",
		"Science is organized common sense where many a beautiful theory was killed by an ugly fact.\n     -Thomas Huxley",
		"The most exciting phrase to hear in science, the one that heralds the most discoveries, is not Eureka! but Thats funny...\n     -Isaac Asimov",
		"If you try and take a cat apart to see how it works, the first thing you have on your hands is a non-working cat.\n     -Douglas Adams",
		"Philosophy of science is about as useful to scientists as ornithology is to birds.\n     -Richard P. Feynman",
		"I have never let my schooling interfere with my education.\n     -Mark Twain",
		"If your result needs a statistician then you should design a better experiment.\n     -Ernest Rutherford",
		"Every generation of humans believed it had all the answers it needed, except for a few mysteries they assumed would be solved at any moment.\nAnd they all believed their ancestors were simplistic and deluded. What are the odds that you are the first generation of humans who will understand reality?\n     -Scott Adams (Dilbert)",
		"In the beginning the Universe was created. This has made a lot of people very angry and been widely regarded as a bad move.\n     -Douglas Adams",
       "The function of science fiction is not always to predict the future but sometimes to prevent it.\n     -Frank Herbert",
		"There ain't no surer way to find out whether you like people or hate them than to travel with them.\n     -Mark Twain",
		"A true friend is someone who is there for you when he'd rather be anywhere else.\n     -Len Wein",
		"Asking dumb questions is easier than corecting dumb mistakes.\n     -Gaussian QC",
		"Sacred cows make the best hamburger.\n     -Mark Twain",
		"Education without common sense is a load of books on the back of an ass.\n     -Gaussian QC",
		"Everything's got a moral, if only you can find it.\n -Lewis Carrol, Alice in Wonderlan",
		"Experience is what you get when you don't get what you want.\n     -Dan Stanford",
		"We have left undone those things which we ought to have done, and we have done those things which we ought not to have done.\n     -Book of common prayer",
		"A fool can ask more questions than a wise man can answer.\n     -Gaussian QC",
		"Mondays are the potholes in the road of life.\n     -Tom Wilson",
		"Actors are so fortunate. They can choose whether they will appear in a tragedy or in comedy, whether they will suffer of make merry, laugh or shed tears. but in real life it is different.\nMost men and women are forced to perform parts for which they have no qualifications. The world is a stage, but the play is badly cast.\n     -Oscar Wilde",
		"In the universe the difficult things are done as if they were easy.\n     -Lao-Tsu",
		"A bird in the hand is safer than one overhead.\n     Newton's seventh law",
		"Be like a postage stamp. Stick to one thing until you get there.\n     -Josh Billings",
		"Steinbach's guidelines for systems programming: never test for an error condition you don't know how to handle.\n     -Gaussian QC",
		"Chinese fortune cookie of Jan 1 1967 say.... all things are difficult before they are easy.\nWe learn so little and forget so much. You will overcome obstacles to achieve success. Ah so.",
		"Getting a simple answer from a professor is like getting a thimble of water from a fire hydrant.\n     -Prof. Len Shapiro, NDSU",
		"Models are to be used, not believed.\n     -Paraphrased by H. Thiel, Principles of Econometrics",
		"If god had meant man to see the sun rise, he would have scheduled it for a later hour.\n     -Gaussian QC",
		"Science at its best provides us with better questions, not absolute answers.\n     -Norman Cousins, 1976",
		"We are reaching the stage where the problems we must solve are going to become insoluble without computers.\nI do not fear computers. I fear the lack of them.\n     -Issac Asimov",
		"Be careful not to become too good of a songbird or theyll throw you into a cage.\n     -Snoopy to Woodstock",
		"The world is made up of the wills, the wonts, and the cants: the wills do everything, the wonts do nothing, the can'ts can't do anything.\n     From Walt Disney's ""Black Hole""",
		"The meek shall inherit the earth. (The rest of us will escape to the stars)\n     -Gaussian QC",
		"All papers that you save will never be needed until such time as they are disposed of, when they become essential.\n     -John Corcoran in Paul Dickson's 'The Official Rules'",
		"There is no subject, however complex, which, if studied with patience and intelligience will not become more complex.\n     -Quoted by D. Gordon Rohman",
		"If you get confused, logic out your dilemma.\n     -Picker X-ray corp. digital printer manual CA. 1964", 
		"To err is human - and to blame it on a computer is even more so.\n     -Gaussian QC",
		"If you want to learn from the theoretical physicists about the methods which they use, I advise you to follow this principle very strictly:/ndont listen to their words; pay attention, instead, to their actions.\n     -A. Einstein, 1934",
		"Those with the gold make the rules.\n     -Peter's golden rule",
		"To suspect your own mortality is to know the beginning of terror. To learn irrefutably that you are mortal is to know the end of terror.\n     -Jessica: Children of Dune by Frank Herbert",
		"A politician is a person who can make waves and then make you think hes the only one who can save the ship.\n     -Ivern Ball",
		"Wisdom is knowing what to do, skill is knowing how to do it, and virtue is not doing it.\n     -Gaussian QC",
		"All science is either physics, or stamp collecting.\n     -Ernest Rutherford, 1871—1937",
		"Theory: supposition which has scientific basis, but not experimentally proven. Fact: a theory which has been proven by enough money to pay for the experiments.\n     -The wizard of ID",
		"It is a capital mistake to theorize before one has data. Insensibly one begins to twist facts to suit theories rather than theories to suit facts.\n     -Sherlock Holmes",
		"Common sense is not so common.\n     -Voltaire",
		"Everybody is ignorant, only on different subjects.\n     -Will Rogers",
		"Unless we change directions, we will wind up where we are headed.\n     -Confucius",
		"The whole of science is nothing more than a refinement of everyday thinking.\n     -A. Einstein",
		"My opinions may have changed, but not the fact that I am right.\n     -Ashleigh Brilliant",
		"Never teach a pig to sing. It wastes your time and annoys the pig.\n     -Seen on a greeting card",
		"Michael Faraday, asked by a politician what good his electrical discoveries were, replied ""at present I do not know, but one day you will be able to tax them.""",
		"A chemical physicist makes precise measurements on impure compounds. A theoretical physical chemist makes imprecise measurements on pure compounds. An experimental physical chemist makes imprecise measurements on impure compounds.\n     -Gaussian QC",
		"KNOWING is a barrier which prevents learning.\n     -Teaching of the Bene Gesserit",
       "You shall know the truth, and the truth shall set you free.\n     -John 8:32",
       "Scientific progress is the discovery of a more and more comprehensive simplicity... The previous successes give us confidence in the future of science: we become more and more conscious of the fact that the universe is cognizable.\n     -Monsignor Georges Lemaître",
       "The miracle of the appropriateness of the language of mathematics for the formulation of the laws of physics is a wonderful gift which we neither understand nor deserve.\n     -Wigner Eugene",
       "The eternal mystery of the world is its comprehensibility…The fact that it is comprehensible is a miracle.\n     -Albert Einstein",
       "Better to illuminate than merely to shine, to deliver to others contemplated truths than merely to contemplate.\n     -Thomas Aquinas",
       "The things that we love tell us what we are.\n     -Thomas Aquinas",
       "Nonsense is nonsense even when spoken by world-famous scientists.\n     -John Lennox",
       "Begin with the beautiful, which leads you to the good, which leads you to the truth.\n     -Robert Barron",
       "Start by doing what's necessary; then do what's possible; and suddenly you are doing the impossible.\n    -Francis of Assisi",
       "The task of the modern educator is not to cut down jungles, but to irrigate deserts.\n     -C.S. Lewis",
       "The future is something which everyone reaches at the rate of 60 minutes an hour, whatever he does, whoever he is.\n     -C.S. Lewis",
       "Failures, repeated failures, are finger posts on the road to achievement. One fails forward toward success.\n     -C.S. Lewis",
       "Yet trees are not 'trees', until so named and seen\n and never were so named, till those had been\n who speech's involuted breath unfurled,\n faint echo and dim picture of the world\n     -J.R.R. Tolkien",
       "The heart of Man is not compound of lies, but draws some wisdom from the only Wise.\n     -J.R.R. Tolkien",
       "For surely “nothing” is every bit as physical as “something,” especially if it is to be defined as the “absence of something.\n     -Lorentz Krauss",
       "Imagine a kettle boiling on a stove. The scientist can tell you much about it. What temperature the water will boil at, the interaction of atoms at different temperatures, the change in the nature of matter and many other interesting and important things.\n But what science can never discover is that that the kettle is on the stove so that you can have a cup of tea with a friend. That is the real reason the you are boiling the kettle and science can't, nor was it ever designed to, tell you that.\n     -Sir John Polkinghorne",
       "It is important to realize that in physics today, we have no knowledge of what energy “is”.\n     -Richard Feynman",
       "THE WORLD IS TOO COMPLICATED IN ALL ITS PARTS AND INTERCONNECTIONS TO BE DUE TO CHANCE ALONE.\n     -Alan Sandage",
       "Logic is the most useful tool of all the arts. Without it no science can be fully known.\n     -William of Ockham",
       "It is pointless to do with more what can be done with fewer.\n     -William of Ockham",
       "The reason man's best friend is a dog is because he wags his tail instead of his tongue.\n     -Anonymus",
       "Wonder is the desire of knowledge.\n    -Thomas Aquinas" ,
       "Characteristics which define beauty are wholeness, harmony and radiance.\n    -Thomas Aquinas"
		]
    

def round_up_to_odd(f):
    #Round to add as blocksize has to be odd
    return int(np.ceil(f) // 2 * 2 + 1)

def os_open(path):
    '''Open file using its associated program in an os (MacOS, Linux, Windows) 
    dependent manner. Exceptions are raised as warnings using a try statement.
    
    path(str): The path of the file to be opened
    '''
    try:
        #Windows
        if sys.platform == "win32":
            os.startfile(path)
        #MacOS
        elif sys.platform == "darwin":
           subprocess.call(["open", path]) 
        #Linux/Unix
        else:
            subprocess.call(["xdg-open", path])
    
    except Exception as e:
        raise Warning(str(e))




def quote(verbose=False):
	
    '''Generates a random quote (most of which are from Gaussian03).
    '''
	 #Generate random index
    index=random.randint(0,len(QUOTE_LIST)-1)
    
    #Get quote
    quote=QUOTE_LIST[index]
    
    #Print if verbose is set to true
    if verbose:
        print_line_by_line(quote)
    
    return quote

def print_line_by_line(string, file=sys.stdout):
    
    string_list=string.split("\n")
    for i in string_list:
        print(i, file)

def warning(string):
    
    print(string, file=sys.stderr)
        

def error(message, exception=None):

    print("Traceback:",file=sys.stderr)
    print(traceback.format_exc(10), file=sys.stderr)

    raise Exception(message, exception)
 

#Class for error handling
class VividException(Exception):
    def __init__(self, message, errors):
			
        super(VividException, self).__init__(message)
        self.errors = errors
    
        print(traceback.format_exc(10), file=sys.stderr)	
        
    def __str__(self):
        
        return repr(self.errors)
    