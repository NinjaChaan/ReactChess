#pragma once
class Ruutu {
public:
	Ruutu(int sarake, int rivi);
	Ruutu();
	int getRivi() const;
	int getSarake() const;
	bool operator==(const Ruutu& other) const;
private:
	int sarake;
	int rivi;
};

