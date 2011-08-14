# word list preparation
wordfile = "./wordscsv"
# file with the words in csv. Spaces, newlines, empty words don't matter. Just no tabs please.
f = File.new(wordfile,"r").read

rawWords = f.gsub("\n",",").gsub(" ", "").split(",").delete_if{|x| x == ""}.length

g = f.gsub!("\n",",").gsub!(" ", "").split(",").delete_if{|x| x == ""}.uniq.sort{|word1, word2| word2.length <=> word1.length}

uqWords= g.length

puts "\n The following words are redundant:\n "

h = g.each do |word1|
	g.each do |word2|
		if word1.include?(word2) then
			next if word1 == word2
			g.delete(word2)
			print "#{word2.upcase}(#{word1.upcase}). "
		end
	end
end

print "\n\n"

h = h.sort{|word1, word2| word1.length <=> word2.length}
h = h.reverse

words = []
cur = ""
h.each do |word|
	if cur.length + word.length < 150 then
		cur << word
		next
	else
		words << cur
		cur = ""
	end
end

puts " Messages have the following lengths:\n\n"

words.each do |word|
	print "#{word.length}, "
end



print "\n\n There are *#{rawWords}* words in the word file.\n \
There are #{[8801].pack("U*")}#{uqWords}#{[8801].pack("U*")} non-duplicate words in the file.\n \
There are #{[10058].pack("U*")}#{h.length}#{[10058].pack("U*")} words in the file after redundant words are eliminated."

print "\n\n #{words.length} messages have been formed from the file.\n"

