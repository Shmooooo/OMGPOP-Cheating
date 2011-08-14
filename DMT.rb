# word list preparation
wordfile = "./wordscsv"
# file with the words in csv. Spaces, newlines, empty words don't matter. Just no tabs please.
f = File.new(wordfile,"r").read

rawWords = f.gsub("\n",",").gsub(" ", "").split(",").delete_if{|x| x == ""}.length

g = f.gsub!("\n",",").gsub!(" ", "").split(",").uniq[1..-1]

uqWords= g.length

h = g.each do |word1|
	g.each do |word2|
		if word1.include?(word2) then
			next if word1 == word2
			g.delete(word2)
			print "#{word1.upcase} incl #{word2.upcase}; kill #{word2.upcase}. "
		end
	end
end

print " There are #{rawWords} words in the word file.\n \
There are #{uqWords} non-duplicate words in the file.\n \
There are #{h.length} words in the file after wholly-contained words are eliminated."

