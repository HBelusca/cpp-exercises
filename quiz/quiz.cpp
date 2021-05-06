// Quiz (C) 2021 HBM
// Uses C++17 features.
//
// Compile: g++ quiz.cpp -o quiz.exe
//
// Usage: quiz.exe <quizfile>
// where <quizfile> specifies the path of a quiz file.

#include <iostream> // For IO streams.
#include <fstream>  // For file streams.
#include <string>	// For std::string
#include <vector>   // For std::vector<...>
#include <limits>   // For std::numeric_limits<...>::max()
#include <utility>  // For std::exchange()

class Question
{
public:
	Question(
		const std::string& question,
		std::size_t answer,
		std::vector<std::string>&& choices);

	// Move-constructor.
	Question(Question&& q) :
		m_question(std::move(q.m_question)),
		m_choices(std::move(q.m_choices)),
		m_answer(std::exchange(q.m_answer, size_t(0)))
	{ }

	virtual ~Question() = default;

	bool Ask(std::ostream& oStr, std::istream& iStr) const;

private:
	const std::string m_question;
	const std::vector<std::string> m_choices;
	std::size_t m_answer;
};

Question::Question(
	const std::string& question,
	std::size_t answer,
	std::vector<std::string>&& choices) :
		m_question(question),
		m_choices(choices),
		m_answer(answer)
{
	// Normalize the answer index (cap'ed by number of choices, and one-based).
	// If zero then question list is empty and no answer is correct.
	m_answer = std::min(std::max(answer, size_t(1)), m_choices.size());
	// m_answer = std::max(std::min(answer, m_choices.size()), size_t(1));
}

bool Question::Ask(
	std::ostream& oStr,
	std::istream& iStr) const
{
	auto answer = 0;
	auto maxAns = std::max(size_t(1), m_choices.size());

	oStr << m_question << std::endl;
	// for (auto choice : m_choices)
	for (auto i = 0; i < m_choices.size(); ++i)
	{
		oStr << (i+1) << ". " << m_choices[i] << std::endl;
	}

	// Loop as long as we don't have a meaningful choice.
	do
	{
		oStr << "Choose 1-" << maxAns << ": ";
		if (!(iStr >> answer))
		{
			// Invalid non-numeric entry:
			// clear stream status, flush it and retry.
			iStr.clear();
			iStr.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
			continue;
		}

	} while (answer < 1 || answer > maxAns);

	oStr << ((answer == m_answer) ? "Correct!" : "Incorrect!") << std::endl << std::endl;

	// Return TRUE or FALSE depending on the correctness,
	// so that the caller can e.g. count the number of correct answers.
	return (answer == m_answer);
}


using namespace std;

int main(int argc, char** argv)
{
	vector<Question> questions;
	size_t score = 0;

	// Only possible command-line format:
	// <program> <quizfile>
	if (argc != 2)
	{
		cout << "Usage: " << argv[0] << " <quizfile>" << endl;
		return -1;
	}

	// Try to open the quiz text file for input.
	ifstream inFile;
	inFile.open(argv[1], ios::in);
	if (!inFile.is_open())
	{
		cerr << "Couldn't open quiz file '" << argv[1] << "'" << endl;
		return -1;
	}

	// Load a question from the input file,
	// together with its list of choices and the answer.
	//
	// Structure of the file:
	//
	// <question line>
	// <answer index (1-based)>
	// <list>
	// <of>
	// <answers>
	// (newline)
	// << other question and answers, or EOF >>

	while (true)
	{
		string question, line;
		vector<string> choices;
		size_t answer;

		// Check for a question.
		getline(inFile, question);

		if (inFile.eof())
			break;

		if (question == "")
			continue;

		// Got a question, check the other lines for index and answers.

		// inFile >> answer;
		getline(inFile, line);
		try
		{
			answer = stoi(line);
		}
		catch (...)
		{
			// Invalid answer format, skip this question.
			continue;
		}

		while (getline(inFile, line))
		{
			// If we have a blank line, the list of answers stops there.
			if (line == "")
				break;
			// Otherwise append the answer to the array.
			choices.push_back(line);
		}

		// Append this new question.
		questions.push_back(move(Question(question, answer, move(choices))));
	}

	inFile.close();

	// Iterate through the questions and ask them, waiting for user answers.
	// Count the score of correct answers.
	for (const auto& question : questions)
	{
		if (question.Ask(cout, cin))
			++score;
	}

	cout << "Your score: " << score << "/" << questions.size() << endl;

	// Destructors automatically called as we get out of scope.
	// questions.clear();
	// If we instead stored pointers to new-allocated objects
	// we would need to explicitly loop over the vector and
	// call the destructors manually, for example:
	// for (const auto& question : questions) { delete question; }

	return 0;
}
