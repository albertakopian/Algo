#include <algorithm>
#include <iostream>
#include <vector>



/* 2_2. Заявки на переговоры
 * В большой IT-фирме есть только одна переговорная комната.
 * Желающие посовещаться заполняют заявки с желаемым временем начала и конца.
 * Ваша задача определить максимальное количество заявок, которое может быть удовлетворено.
 * Число заявок ≤ 100000.
 */

class RequestsForMeeting {
public:
	void AddRequest(const int begin, const int end);

	size_t GetMaxSatisfiedRequestsNumber();

private:
	struct Request;

	std::vector<Request> requests_;
};

struct RequestsForMeeting::Request {
	Request(const int begin, const int end)
		: begin{begin}, end{end} {}

	bool operator< (const Request& other) {
		return end < other.end;
	}

	int begin = 0;
	int end = 0;
};

void RequestsForMeeting::AddRequest(const int begin, const int end) {
	requests_.push_back(Request(begin, end));
}

size_t RequestsForMeeting::GetMaxSatisfiedRequestsNumber() {
	if (requests_.empty()) {
		return 0;
	}
	std::sort(requests_.begin(), requests_.end());
	size_t answer = 1;
	int last_end = requests_[0].end;
	for (int i = 1; i < requests_.size(); ++i) {
		if (requests_[i].begin >= last_end) {
			++answer;
			last_end = requests_[i].end;
		}
	}
	return answer;
}

int main() {
	RequestsForMeeting requests;
	int begin = 0;
	int end = 0;
	while (std::cin >> begin >> end) {
		requests.AddRequest(begin, end);
	}
	std::cout << requests.GetMaxSatisfiedRequestsNumber() << std::endl;
	return 0;
}
