#include "Rectangle.h"
#include "Music.h"
#include "Pet.h"

using namespace std;
int main() {
	// Rectangle 1 객체 생성
	Rectangle rect1(3.4, 2.5);
	cout << "Rectangle 1:" << endl;
	// printf("Rectangle 1:\n"); 윗줄이랑 똑같음
	cout << "Area: " << rect1.getArea() << endl;
	//printf("Area: %lf , rect1.getArea());
	cout << "Perimeter: " << rect1.getPerimeter() << endl;
	cout << "Is Square ?: " << boolalpha << rect1.isSquare() << endl;
	printf("========================================================\n");

	// Pet 클래스 테스트
	Pet myPet("kong", 5, "Cat");
	cout << "My Pet's name is " << myPet.getName() << endl;
	// Puppy 클래스 테스트
	Puppy myPuppy("Lucky", 10, "Dog", "Puddle");
	cout << "My puppy's breed is " << myPuppy.getBreed() << endl;
	printf("========================================================\n");

	//Streaming service 테스트
	MusicStreamingService myService("spotify");
	//음악 Service에 추가하기
	myService.addMusic("SWIM", "BTS", "ARIRANG", 2026);
	myService.addMusic("RUDE", "Heartrs2Hearts", "a", 2026);
	myService.addMusic("BANGBANG", "IVE", "REVIVE", 2026);
	myService.addMusic("JUMP", "BlackPink", "Jump", 2025);
	myService.addMusic("ShutDown", "BlackPing", "BornPink", 2022);
	//title로 search
	string music_title;
	cout << "Enter the Music Title: ";
	cin >> music_title;
	//scanf_s("%s", &music_title); 윗줄이랑 같음
	Music* result = myService.searchByTitle(music_title);
	if(result != NULL) {
		cout << "Found: " << result->getTitle() << "by" << result->getArtist() << endl;
	}
	else {
		cout << "Not Found" << endl;

	}

};