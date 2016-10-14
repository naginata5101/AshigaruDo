#include <Mikan.h>
#include <time.h>
#include <vector>

#define mHEI 96
#define CHIP 48
#define WIDTH 640
#define HEIGHT 384
#define NOSELECT -1;

using namespace std;

const int PlayerX = CHIP;
const int maxSpeed = 10;	//プレイヤーのy軸方向の最高速
int scrollSpeed;	//強制スクロールの速さ

enum Weapon{
	FIST = 1,
	SWORD,
	KNIFE,
	SPEAR,
	HALBERD,
	ROCK,
	BOW,
	SHIELD,
	//以降　刀語
	KANNA = 10,
	NAMAKURA,
	TSURUGI,
	HARI,
	YOROI,
	KANADUTI,
	BITA,
	KANZASHI,
	NOKOGIRI,
	HAKARI,
	MEKKI,
	JUU,
	YASURI = 30,
	SHINAI	//刀を持った虚刀流
};
enum class Hit {
	NO, SIDE, UP, DOWN
};
enum class Game {
	TITLE, NORMAL, GATARI, KYOTOU, TRAIN, RESULT, PRE_TITLE
};

//
void makeBlock();
void makeItem();
void removeItem();
void removeBlock();
void getItem();
Hit hitBlock();
int breakBlock(Weapon w);
//
bool gameInit();
void drawTitle();
//
void normalUpdate();
void gatariUpdate();
void kyotoUpdate();
void trainUpdate();
void drawGame();
//
void resultUpdate();
void drawResult();
//
void goTitle();


class Block {
public:	int x;
public:	int y;
public: int w;
public: int h;
public: int c;	//色、何種類か用意しておく
public: static int const maxC = 3;

public: Block(int Y, int W, int C) {
			x = WIDTH;
			y = Y;
			w = W;
			h = mHEI;
			c = C;
		}

public: void move() {
			x -= scrollSpeed;
		}

public: void draw() {
			int f = 3;
			int f2 = f * 2;

			MikanDraw->DrawBox(x, y, w, mHEI, 0xFF000000);

			switch(c) {
			case 0:
				MikanDraw->DrawBox(x + f, y + f, w - f2, h - f2, 0xFF555555);
				break;

			case 1:
				MikanDraw->DrawBox(x + f, y + f, w - f2, h - f2, 0xFF882222);
				break;

			case 2:
				MikanDraw->DrawBox(x + f, y + f, w - f2, h - f2, 0xFF228822);
				break;

			default:
				MikanDraw->DrawBox(x + f, y + f, w - f2, h - f2, 0xFF222288);
				break;
			}
		}
};

class iWeapon {
public:	int x;
public:	int y;
public: int w;
public: int h;
public:	Weapon t;

public: iWeapon(int Y, Weapon T) {
			x = WIDTH;
			y = Y;
			w = h = CHIP;
			t = T;
		}

public: void move() {
			x -= scrollSpeed;
		}

public: void draw() {
			int c = CHIP;

			//周りを黒い線で囲う
			MikanDraw->DrawBoxLine(x - 1, y - 1, c + 1, c + 1, 0xFF000000);
			if (t < 10) {
				MikanDraw->DrawTexture(0, x, y, t * c, 0, c, c);
			} else if (t < 30) {
				MikanDraw->DrawTexture(0, x, y, (t - 10) * c, c, c, c);
			} else if (t == YASURI) {
				MikanDraw->DrawTexture(0, x, y, 1 * c, 0, c, c);
			} else if (t == SHINAI) {
				MikanDraw->DrawTexture(0, x, y, 9 * c, 0, c, c);
			}
			/*
				switch (t)
				{
				case KANNA:
					MikanDraw->Printf(2, x, y, "鉋");
					break;
				case NAMAKURA:
					MikanDraw->Printf(2, x, y, "鈍");
					break;
				case TSURUGI:
					MikanDraw->Printf(3, x, y + c / 4, "金殺");
					break;
				case HARI:
					MikanDraw->Printf(2, x, y, "針");
					break;
				case YOROI:
					MikanDraw->Printf(2, x, y, "鎧");
					break;
				case KANADUTI:
					MikanDraw->Printf(2, x, y, "鎚");
					break;
				case BITA:
					MikanDraw->Printf(2, x, y, "鐚");
					break;
				case KANZASHI:
					MikanDraw->Printf(2, x, y, "釵");
					break;
				case NOKOGIRI:
					MikanDraw->Printf(2, x, y, "鋸");
					break;
				case HAKARI:
					MikanDraw->Printf(2, x, y, "銓");
					break;
				case MEKKI:
					MikanDraw->Printf(2, x, y, "鍍");
					break;
				case JUU:
					MikanDraw->Printf(2, x, y, "銃");
					break;
				}
				*/
		}
};

class eWeapon {
	//左上の座標、プレイヤーの右上基準
public: Weapon t;	//武器の種類
public: int x;
public:	int y;	//真中は35?
public:	int w;
public:	int h;
public:	int n;	//使用回数
public: int i;	//攻撃間隔(interval)
public: string name;
private: int py;
private: int ph;

public: eWeapon(Weapon type, int playerX, int playerY, int playerW, int playerH) {
			x = playerX + playerW;
			py = playerY;
			ph = playerH;
			t = type;

			switch(t) {
			case SWORD:
				y = 10;
				w = 40;
				h = 50;
				n = 25;
				i = 60;
				name = "太刀";
				break;

			case KNIFE:
				y = 20;
				w = 30;
				h = 30;
				n = 40;
				i = 40;
				name = "脇差";
				break;

			case SPEAR:
				y = 28;
				w = 100;
				h = 14;
				n = 25;
				i = 40;
				name = "長槍";
				break;

			case HALBERD:
				y = -35;
				w = 80;
				h = 140;
				n = 15;
				i = 70;
				name = "薙刀";
				break;

			case ROCK:
				y = 31;
				w = WIDTH - x;
				h = 8;
				n = 40;
				i = 60;
				name = "石礫";
				break;

			case BOW:
				y = 31;
				w = WIDTH - x;
				h = 8;
				n = 15;
				i = 90;
				name = "強弓";
				break;

			case SHIELD:
				y = 1;
				w = 5;
				h = ph - 2;
				n = 10;
				i = 0;
				name = "竹束";
				break;

			case KANNA:
				y = 10;
				w = 40;
				h = 50;
				n = 60;
				i = 60;
				name = "鉋";
				break;

			case NAMAKURA:
				y = 10;
				w = 40;
				h = 50;
				n = 25;
				i = 35;
				name = "鈍";
				break;

			case TSURUGI:
				y = 10;
				w = WIDTH - x;
				h = 50;
				n = 30;
				i = 60;
				name = "金殺";
				break;

			case HARI:
				y = -10;
				w = 40;
				h = 90;
				n = 10;
				i = 40;
				name = "針";
				break;

			case YOROI:
				y = 1;
				w = 5;
				h = ph - 2;
				n = 25;
				i = 0;
				name = "鎧";
				break;

			case KANADUTI:
				y = -35;
				w = 80;
				h = 140;
				n = 60;
				i = 100;
				name = "鎚";
				break;

			case NOKOGIRI:
				y = 10;
				w = 40;
				h = 50;
				n = 45;
				i = 40;
				name = "鋸";
				break;

			case HAKARI:
				y = -HEIGHT;
				w = WIDTH * 2;
				h = HEIGHT * 2 + mHEI;
				n = 1;
				i = 0;
				name = "銓";
				break;

			case MEKKI:
				y = -10;
				w = 40;
				h = 90;
				n = 25;
				i = 40;
				name = "鍍";
				break;

			case JUU:
				y = 31;
				w = WIDTH - x;
				h = 8;
				n = 17;
				i = 15;
				name = "銃";
				break;

			case YASURI:
				y = -10;
				w = 50;
				h = 90;
				n = 1000000;
				i = 15;
				name = "鑢";
				break;

			case SHINAI:
				y = 20;
				w = 30;
				h = 30;
				n = 5;
				i = 100;
				name = "竹刀";
				break;

			default:	//デフォルトは拳
				y = 30;
				w = 10;
				h = 10;
				n = 1000000;	//無限ってどうしよう？
				i = 40;
				name = "鉄拳";
				break;
			}
		}

public: void update(int playerX, int playerY, int playerW, int playerH) {
			x = playerX + playerW;
			py = playerY;
			ph = playerH;
		}

public: void draw(int interval) {
			if (i == interval) {
				MikanDraw->DrawBox(x, py + y, w, h, 0xAAFFFF00);
			} else if (0 < interval) {
				MikanDraw->DrawBox(x, py + y, w, h, 0x88FF0000);
			} else {
				MikanDraw->DrawBox(x, py + y, w, h, 0x880000FF);
			}
		}
};

class Player {
public:	int x;
public:	int y;
public:	int w;
public:	int h;
public: int life;
public: int score;
private: int vy;
private: int jumpTime;
private: int interval;
private: int damageTime;	//正ならば、直前にダメージを受けてからの時間
		 //負ならば、残りの無敵時間
private: int normalHighScore;
private: int gatariHighScore;
private: int kyotouHighScore;
public: bool equi1;
public: bool choiceFlag;	//choiceWeapon用
public: eWeapon *e1;
public: eWeapon *e2;

public: Player(Game g) {
			w = CHIP;
			h = CHIP * 3 / 2;
			x = CHIP;
			y = mHEI * 4 - h;
			life = 5;
			score = 0;
			vy = 0;
			if (g == Game::NORMAL) {
				e1 = new eWeapon(SWORD, x, y, w, h);
			} else if (g == Game::GATARI) {
				e1 = new eWeapon(KANNA, x, y, w, h);
			} else if (g == Game::KYOTOU) {
				e1 = new eWeapon(YASURI, x, y, w, h);
			} else {
				e1 = new eWeapon(FIST, x, y, w, h);
			}
			e2 = new eWeapon(FIST, x, y, w, h);
			equi1 = true;
			choiceFlag = false;
			interval = e1->i;
			damageTime = 1000;
			normalHighScore = gatariHighScore = kyotouHighScore = 0;

			FILE *fp = fopen("score.txt", "r");
			if (fp != NULL) {
				fscanf(fp, " 　通常戦闘　  : %4d点\n 『刀語』戦闘  : %4d点\n「虚刀流」戦闘 : %4d点",
					&normalHighScore, &gatariHighScore, &kyotouHighScore);
				fclose(fp);
			}
		}

private: void jump(Hit h) {
			 int input = MikanInput->GetKeyNum(K_X);
			 eWeapon *e;
			 if (equi1 == true) {
				 e = e1;
			 } else {
				 e = e2;
			 }

			 if (h == Hit::UP) {
				 jumpTime = 10;
				 vy = 0;
			 } else if (h == Hit::DOWN) {
				 jumpTime = 0;
			 }

			 if (0 < input && 0 < jumpTime && e->t != NOKOGIRI) {
				 vy = -10;
				 --jumpTime;
				 MikanSound->Play(4, false);
			 } else if (input == -1) {
				 jumpTime = 0;
			 }
		 }

private: Hit move() {
			 y += vy;
			 ++vy;
			 if (vy > maxSpeed) {
				 vy = maxSpeed;
			 }

			 if (HEIGHT <= y + h) {
				 y = HEIGHT - h;
				 return Hit::UP;
			 } else if (y < 0) {
				 y = 0;
				 return Hit::DOWN;
			 } else {
				 return Hit::NO;
			 }
		 }

private: void attack() {
			 int addScore = 0;
			 eWeapon *e;
			 if (equi1 == true) {
				 e = e1;
			 } else {
				 e = e2;
			 }
			 --interval; //黄色四角を表示するために、この処理は上に書く

			 if (interval <= 0) {
				 if (MikanInput->GetKeyNum(K_Z) > 0 || e->t == MEKKI) {
					 if (equi1 == true) {
						 interval = e1->i;
						 addScore = breakBlock(e1->t);
						 if (addScore != 0) {
							 --e1->n;
						 }
					 } else {
						 interval = e2->i;
						 addScore = breakBlock(e2->t);
						 if (addScore != 0) {
							 --e2->n;
						 }
					 }
					 MikanSound->Play(3, false);
				 }
			 }
			 score += addScore;
		 }

private: void breakWeapon(Game g) {
			 if (g == Game::KYOTOU) {
				 if (e1->n <= 0) {
					 delete e1;
					 e1 = new eWeapon(YASURI, x, y, w, h);
					 interval = e1->i;
				 }
			 } else {
				 if (e1->n <= 0) {
					 delete e1;
					 e1 = new eWeapon(FIST, x, y, w, h);
					 interval = e1->i;
				 }
				 if (e2->n <= 0) {
					 delete e2;
					 e2 = new eWeapon(FIST, x, y, w, h);
					 interval = e2->i;
				 }
			 }
		 }

private: void changeEqui() {
			 if (equi1 == true && MikanInput->GetKeyNum(K_DOWN) == 1) {
				 equi1 = false;
				 interval = e2->i;
				 MikanSound->Play(5, false);
			 }
			 if (equi1 == false && MikanInput->GetKeyNum(K_UP) == 1) {
				 equi1 = true;
				 interval = e1->i;
				 MikanSound->Play(5, false);
			 }
		 }

public: void update(Game g) {
			Hit hs, hb;

			hs = move();
			hb = hitBlock();
			getItem();
			if (hs == Hit::UP || hb == Hit::UP) {
				jump(Hit::UP);
			} else if (hs == Hit::DOWN || hb == Hit::DOWN) {
				jump(Hit::DOWN);
			} else {
				jump(Hit::NO);
			}
			if (hb == Hit::SIDE && damageTime >= 0) {
				--life;
				damageTime = 0;
				MikanSound->Play(2, false);
			}

			breakWeapon(g);
			if (g != Game::KYOTOU) {
				changeEqui();
			}
			e1->update(x, y, w, h);
			e2->update(x, y, w, h);
			attack();

			if (damageTime < 10000) {	//念の為、オバーフローしないように
				++damageTime;
			}
		}

public: void equipment(Weapon W) {
			if (W == BITA) {
				++life;
			} else if (W == KANZASHI) {
				damageTime = -300;
			} else {
				if (equi1 == true) {
					delete e1;
					e1 = new eWeapon(W, x, y, w, h);
				} else {
					delete e2;
					e2 = new eWeapon(W, x, y, w, h);
				}
				interval = 0;
				MikanSound->Play(5, false);
			}
		}

public: void choiceWeapon() {
			if (MikanInput->GetKeyNum(K_0) == 1 && choiceFlag == false) {
				choiceFlag = true;
			} else if (MikanInput->GetKeyNum(K_1) == 1) {
				delete e1;
				if (choiceFlag == false) {
					e1 = new eWeapon(FIST, x, y, w, h);
				} else {
					e1 = new eWeapon(KANNA, x, y, w, h);
					choiceFlag = false;
				}
			} else if (MikanInput->GetKeyNum(K_2) == 1) {
				delete e1;
				if (choiceFlag == false) {
					e1 = new eWeapon(SWORD, x, y, w, h);
				} else {
					e1 = new eWeapon(NAMAKURA, x, y, w, h);
					choiceFlag = false;
				}
			} else if (MikanInput->GetKeyNum(K_3) == 1) {
				delete e1;
				if (choiceFlag == false) {
					e1 = new eWeapon(KNIFE, x, y, w, h);
				} else {
					e1 = new eWeapon(TSURUGI, x, y, w, h);
					choiceFlag = false;
				}
			} else if (MikanInput->GetKeyNum(K_4) == 1) {
				delete e1;
				if (choiceFlag == false) {
					e1 = new eWeapon(SPEAR, x, y, w, h);
				} else {
					e1 = new eWeapon(HARI, x, y, w, h);
					choiceFlag = false;
				}
			} else if (MikanInput->GetKeyNum(K_5) == 1) {
				delete e1;
				if (choiceFlag == false) {
					e1 = new eWeapon(HALBERD, x, y, w, h);
				} else {
					e1 = new eWeapon(YOROI, x, y, w, h);
					choiceFlag = false;
				}
			} else if (MikanInput->GetKeyNum(K_6) == 1) {
				delete e1;
				if (choiceFlag == false) {
					e1 = new eWeapon(ROCK, x, y, w, h);
				} else {
					e1 = new eWeapon(KANADUTI, x, y, w, h);
					choiceFlag = false;
				}
			} else if (MikanInput->GetKeyNum(K_7) == 1) {
				delete e1;
				if (choiceFlag == false) {
					e1 = new eWeapon(BOW, x, y, w, h);
				} else {
					e1 = new eWeapon(NOKOGIRI, x, y, w, h);
					choiceFlag = false;
				}
			} else if (MikanInput->GetKeyNum(K_8) == 1) {
				delete e1;
				if (choiceFlag == false) {
					e1 = new eWeapon(SHIELD, x, y, w, h);
				} else {
					e1 = new eWeapon(HAKARI, x, y, w, h);
					choiceFlag = false;
				}
			} else if (MikanInput->GetKeyNum(K_9) == 1 && choiceFlag != false) {
				delete e1;
				e1 = new eWeapon(MEKKI, x, y, w, h);
				choiceFlag = false;
			} else if (MikanInput->GetKeyNum(K_0) == 1 && choiceFlag != false) {
				delete e1;
				e1 = new eWeapon(JUU, x, y, w, h);
				choiceFlag = false;
			}
		}

public: void draw(Game g) {
			int c = CHIP; 
			int sp = c * 9 + 10;	//下、StartPoint
			int highScore;
			int t;

			if (g == Game::NORMAL) {
				highScore = normalHighScore;
			} else if (g == Game::GATARI) {
				highScore = gatariHighScore;
			} else if (g == Game::KYOTOU) {
				highScore = kyotouHighScore;
			}

			if (damageTime < 0) {	//無敵時間
				MikanDraw->DrawTexture(0, x, y, c * 2, c * 2, w, h);
			} else if (damageTime < 30) {	//ダメージ直後
				MikanDraw->DrawTexture(0, x, y, c * 1, c * 2, w, h);
			} else {
				MikanDraw->DrawTexture(0, x, y, c * 0, c * 2, w, h);
			}
			if (equi1 == true) {
				e1->draw(interval);
			} else {
				e2->draw(interval);
			}


			//下
			if (equi1 == true) {
				MikanDraw->DrawBox(sp, HEIGHT + 0, c * 4, c, 0xFFAAAA00);
			} else {
				MikanDraw->DrawBox(sp, HEIGHT + c, c * 4, c, 0xFFAAAA00);
			}

			if (g != Game::TRAIN) {
				MikanDraw->Printf(0, 0, HEIGHT + 0, "体力\n %02d", life);
				MikanDraw->Printf(0, c * 2 + 20, HEIGHT + 0, "  得点  %4d", score);
				MikanDraw->Printf(0, c * 2 + 20, HEIGHT + c, "最高得点%4d", highScore);
			} else {
				if (choiceFlag == false) {
					MikanDraw->Printf(0, c * 2 + 20, HEIGHT, "通常武器");
				} else {
					MikanDraw->Printf(0, c * 2 + 20, HEIGHT, "完成形変体刀");
				}
			}


			t = e1->t;
			if (t == FIST || t == YASURI) {
				MikanDraw->Printf(0, sp + c * 0, HEIGHT + 0, "∞");
			} else {
				MikanDraw->Printf(0, sp + c * 0, HEIGHT + 0, "%d", e1->n);
			}
			if (t < 10) {
				MikanDraw->DrawTexture(0, sp + c * 1, HEIGHT + 0, t * c, 0, c, c);
			} else if (t < 30) {
				MikanDraw->DrawTexture(0, sp + c * 1, HEIGHT + 0, (t - 10) * c, c, c, c);
			} else if (t == YASURI) {
				MikanDraw->DrawTexture(0, sp + c * 1, HEIGHT + 0, 1 * c, 0, c, c);
			} else if (t == SHINAI) {
				MikanDraw->DrawTexture(0, sp + c * 1, HEIGHT + 0, 9 * c, 0, c, c);
			}
			MikanDraw->Printf(0, sp + c * 2, HEIGHT + 0, "%s", e1->name.c_str());

			if (g != Game::KYOTOU) {
				t = e2->t;
				if (t == FIST || t == YASURI) {
					MikanDraw->Printf(0, sp + c * 0, HEIGHT + c, "∞");
				} else {
					MikanDraw->Printf(0, sp + c * 0, HEIGHT + c, "%d", e2->n);
				}
				if (t < 10) {
					MikanDraw->DrawTexture(0, sp + c * 1, HEIGHT + c, t * c, 0, c, c);
				} else if (t < 30) {
					MikanDraw->DrawTexture(0, sp + c * 1, HEIGHT + c, (t - 10) * c, c, c, c);
				} else if (t == YASURI) {
					MikanDraw->DrawTexture(0, sp + c * 1, HEIGHT + c, 1 * c, 0, c, c);
				} else if (t == SHINAI) {
					MikanDraw->DrawTexture(0, sp + c * 1, HEIGHT + c, 9 * c, 0, c, c);
				}
				MikanDraw->Printf(0, sp + c * 2, HEIGHT + c, "%s", e2->name.c_str());
			}
		}

public: int saveFile(Game g) {
			int highScore = 0;

			if (g == Game::NORMAL) {
				if (normalHighScore <= score) {
					normalHighScore = score;
				}
				highScore = normalHighScore;
			} else if (g == Game::GATARI) {	
				if (gatariHighScore <= score) {
					gatariHighScore = score;
				}
				highScore = gatariHighScore;
			} else if (g == Game::KYOTOU) {	
				if (kyotouHighScore <= score) {
					kyotouHighScore = score;
				}
				highScore = kyotouHighScore;
			}

			FILE *fp = fopen("score.txt", "w");
			if (fp != NULL) {
				fprintf(fp, " 　通常戦闘　  : %4d点\n 『刀語』戦闘  : %4d点\n「虚刀流」戦闘 : %4d点",
					normalHighScore, gatariHighScore, kyotouHighScore);
				fclose(fp);
			}
			return highScore;
		}

public: void allDelete() {
			delete e1;
			delete e2;
		}
};


Player player = Player(Game::TITLE);
vector<Block> block;
vector<iWeapon> item;

int choice = 0;
Game game = Game::TITLE;
int timer;
int highScore;	//リザルト画面用

//
void makeBlock() {
	bool flag;
	int y = rand() % 4;
	int w = mHEI + rand() % (mHEI * 3);
	int c = rand() % Block::maxC;

	for (int i = 0; i < 4; i++) {
		flag = true;
		for (int j = 0; j < block.size(); j++) {
			Block b = block.at(j);
			if (WIDTH < b.x + b.w && y * mHEI == b.y) {
				flag = false;
			}
		}
		if (flag == true) {
			block.push_back(Block(y * mHEI, w, c));
			return;
		}

		if (y >= 3) {
			y = 0;
		} else {
			++y;
		}
	}
}

void makeItem(Game game) {
	int y = rand() % (HEIGHT - CHIP);
	Weapon t;
	int r;
	if (game == Game::NORMAL) {
		r = 1 + 1 + rand() % 7;
	} else if (game == Game::GATARI) {
		r = 1 + 1 + 7 + rand() % 12;
	} else if (game == Game::KYOTOU) {
		r = 30;
	}

	switch (r)
	{
	case 2:
		t = SWORD;
		break;

	case 3:
		t = KNIFE;
		break;

	case 4:
		t = SPEAR;
		break;

	case 5:
		t = HALBERD;
		break;

	case 6:
		t = ROCK;
		break;

	case 7:
		t = BOW;
		break;

	case 8:
		t = SHIELD;
		break;

	case 9:
		t = KANNA;
		break;

	case 10:
		t = NAMAKURA;
		break;

	case 11:
		t = TSURUGI;
		break;

	case 12:
		t = HARI;
		break;

	case 13:
		t = YOROI;
		break;

	case 14:
		t = KANADUTI;
		break;

	case 15:
		t = BITA;
		break;

	case 16:
		t = KANZASHI;
		break;

	case 17:
		t = NOKOGIRI;
		break;

	case 18:
		t = HAKARI;
		break;

	case 19:
		t = MEKKI;
		break;

	case 20:
		t = JUU;
		break;

	default:
		t = SHINAI;
		break;
	}

	item.push_back(iWeapon(y, t));
}

void removeItem() {
	for (int i = 0; i < item.size(); i++) {
		iWeapon w = item.at(i);
		if (w.x + w.w < 0) {
			item.erase(item.begin() + i);
		}
	}
}

void removeBlock() {
	for (int i = 0; i < block.size(); i++) {
		Block b = block.at(i);
		if (b.x + b.w < 0) {
			block.erase(block.begin() + i);
		}
	}
}

void getItem() {
	int px = player.x;
	int py = player.y;
	int pw = player.w;
	int ph = player.h;

	for (int i = 0; i < item.size(); i++) {
		iWeapon w = item.at(i);
		if (px <= w.x + w.w && w.x <= px + pw && py <= w.y + w.h && w.y <= py + ph) {
			item.erase(item.begin() + i);
			player.equipment(w.t);
		}
	}
}

Hit hitBlock() {
	int px = player.x;
	int py = player.y;
	int pw = player.w;
	int ph = player.h;
	int space = maxSpeed + 1;
	Hit hit = Hit::NO;			//優先順位4

	for (int i = 0; i < block.size(); i++) {
		Block b = block.at(i);

		if (px <= b.x + b.w && b.x <= px + pw && py <= b.y + b.h && b.y <= py + ph) {
			if (py + ph <= b.y + space && hit != Hit::SIDE) {
				player.y = b.y - ph;
				hit = Hit::UP;	//優先順位2
			} else if (b.y + b.h <= py + space && hit == Hit::NO) {
				player.y = b.y + b.h;
				hit = Hit::DOWN;	//優先順位3
			} else {
				block.erase(block.begin() + i);
				hit = Hit::SIDE;	//優先順位1
			}
		}
	}
	return hit;
}

int breakBlock(Weapon w) {
	int wx;
	int wy = player.y;	//基準点
	int ww;
	int wh;
	int flag = 0;

	if (player.equi1 == true) {
		wx = player.e1->x;
		wy += player.e1->y;
		ww = player.e1->w;
		wh = player.e1->h;
	} else {
		wx = player.e2->x;
		wy += player.e2->y;
		ww = player.e2->w;
		wh = player.e2->h;
	}

	if (w != ROCK) {
		for (int i = block.size() - 1; 0 <= i ; i--) {
			//vector::erase()によってブロックを消す際に、残りの要素は前に詰められる。
			//そのまま降順に当たり判定を行なっていくと、
			//消した直後のブロックが飛ばされることになる。
			//（直後のブロックが、直前に判定をおこなった番号になっているから）
			//そこで降順
			Block b = block.at(i);
			if (wx <= b.x + b.w && b.x <= wx + ww && wy <= b.y + b.h && b.y <= wy + wh) {
				block.erase(block.begin() + i);
				flag += (4 - b.y / mHEI) * 10;
				MikanSound->Play(1, false);
			}
		}
	} else {
		//石礫の場合「一番前のブロックを"1つだけ"消す」なので昇順にする。
		for (int i = 0; i < block.size(); i++) {
			Block b = block.at(i);
			if (wx <= b.x + b.w && b.x <= wx + ww && wy <= b.y + b.h && b.y <= wy + wh) {
				block.erase(block.begin() + i);
				flag += (4 - b.y / mHEI) * 10;
				MikanSound->Play(1, false);
				break;
			}
		}
	}
	return flag;
}

//
bool gameInit() {
	if (MikanInput->GetKeyNum(K_Z) == 1) {
		switch (choice)
		{
		case 0:
			game = Game::NORMAL;
			scrollSpeed = 3;
			break;

		case 1:
			game = Game::GATARI;
			scrollSpeed = 6;
			break;

		case 2:
			game = Game::KYOTOU;
			scrollSpeed = 6;
			break;

		case 3:
			game = Game::TRAIN;
			break;

		default:
			return false;
			break;
		}

		timer = 0;

		player = Player(game);
		MikanSound->Play(5, false);
		MikanSound->Play(10, true);
	}
	if (MikanInput->GetKeyNum(K_UP) == 1) {
		if (choice == 0) {
			choice = 4;
		} else {
			--choice;
		}
		MikanSound->Play(0, false);
	}
	if (MikanInput->GetKeyNum(K_DOWN) == 1) {
		if (choice == 4) {
			choice = 0;
		} else {
			++choice;
		}
		MikanSound->Play(0, false);
	}
	return true;
}

void drawTitle() {
	MikanDraw->DrawBox(0, 200 + 50 * choice, WIDTH, CHIP, 0xFFAA0000);
	MikanDraw->Printf(1, (WIDTH - CHIP * 2 * 3) / 2 ,50, "足軽道");
	MikanDraw->Printf(0, (WIDTH - CHIP * 4) / 2 , 200, "通常戦闘");
	MikanDraw->Printf(0, (WIDTH - CHIP * 7) / 2 , 250, "『刀語』戦闘");
	MikanDraw->Printf(0, (WIDTH - CHIP * 8) / 2 , 300, "「虚刀流」戦闘");
	MikanDraw->Printf(0, (WIDTH - CHIP * 2) / 2 , 350, "特訓");
	MikanDraw->Printf(0, (WIDTH - CHIP * 2) / 2 , 400, "終了");
}

//
void normalUpdate() {
	if (player.life <= 0) {
		game = Game::RESULT;
		highScore = player.saveFile(Game::NORMAL);
	}

	if (timer % (60 - timer / 1000) == 0) {
		makeBlock();
	}
	if (timer % (60 * 15) == 0) {
		makeItem(game);
	}

	for (int i = 0; i < item.size(); i++) {
		item.at(i).move();
	}
	for (int i = 0; i < block.size(); i++) {
		block.at(i).move();
	}
	player.update(game);

	removeItem();
	removeBlock();

	++timer;
}

void gatariUpdate() {
	if (player.life <= 0) {
		game = Game::RESULT;
		highScore = player.saveFile(Game::GATARI);
	}

	if (timer % (40 - timer / 1000) == 0) {
		makeBlock();
	}
	if (timer % (60 * 6) == 0) {
		makeItem(game);
	}

	for (int i = 0; i < item.size(); i++) {
		item.at(i).move();
	}
	for (int i = 0; i < block.size(); i++) {
		block.at(i).move();
	}
	player.update(game);

	removeItem();
	removeBlock();

	++timer;
}

void kyotouUpdate() {
	if (player.life <= 0) {
		game = Game::RESULT;
		highScore = player.saveFile(Game::KYOTOU);
	}

	if (timer % (30 - timer / 1000) == 0) {
		makeBlock();
	}
	if (timer % (60 * 4) == 0) {
		makeItem(game);
	}

	for (int i = 0; i < item.size(); i++) {
		item.at(i).move();
	}
	for (int i = 0; i < block.size(); i++) {
		block.at(i).move();
	}
	player.update(game);

	removeItem();
	removeBlock();

	++timer;
}

void trainUpdate() {
	if (MikanInput->GetKeyNum(K_SPACE) > 0) {
		game = Game::PRE_TITLE;
	}

	player.update(game);
	player.choiceWeapon();
}

void drawGame(Game g) {
	//背景
	MikanDraw->DrawBox(0, 0, WIDTH, HEIGHT, MikanDraw->Color(255, 153, 217, 234));
	MikanDraw->DrawBox(CHIP * 2 + 15, HEIGHT, 13 * CHIP / 2 + 10, mHEI, 0xFF00AA00);

	player.draw(g);
	for (int i = 0; i < block.size(); i++) {
		block.at(i).draw();
	}
	for (int i = 0; i < item.size(); i++) {
		item.at(i).draw();
	}
}

//
void resultUpdate() {
	if (MikanInput->GetKeyNum(K_SPACE) == 1) {
		game = Game::PRE_TITLE;
		MikanSound->Play(5, false);
	}
}

void drawResult(){
	MikanDraw->Printf(0, (WIDTH - (CHIP / 2) * 17) / 2, 100, "最高得点 : %4d点", highScore);
	MikanDraw->Printf(0, (WIDTH - (CHIP / 2) * 17) / 2, 150, "  得点   : %4d点", player.score);
	MikanDraw->Printf(0, (WIDTH - CHIP * 12) / 2, 300, "スペースキーで選択画面へ");
}

//
void goTitle() {
	game = Game::TITLE;
	item.clear();
	block.clear();
	choice = 0;
	MikanSound->Stop(10);
}


//Mikan---------------------------------------------
void SystemInit(void) {
	MikanWindow->SetWindowName("足軽道");
	MikanWindow->SetWindowIcon("GAME_ICON");
}

void UserInit(void) {
	srand((unsigned)time(NULL));
	MikanDraw->CreateTexture(0, NULL, "TEXTURE", TRC_ZERO);
	MikanDraw->CreateFont(0, "ＭＳ ゴシック", CHIP, 0xFFFFFFFF);
	MikanDraw->CreateFont(1, "ＭＳ ゴシック", CHIP * 2, 0xFFFFFF00);
	MikanDraw->CreateFont(2, "ＭＳ ゴシック", CHIP, 0xFF000000);
	MikanDraw->CreateFont(3, "ＭＳ ゴシック", CHIP / 2, 0xFF000000);
	MikanSound->Load(10, NULL, "BGM");
	MikanSound->Load(0, NULL, "SE1");
	MikanSound->Load(1, NULL, "SE2");
	MikanSound->Load(2, NULL, "SE3");
	MikanSound->Load(3, NULL, "SE4");
	MikanSound->Load(4, NULL, "SE5");
	MikanSound->Load(5, NULL, "SE6");
	MikanSound->SetVolume(10, 100);
}

int MainLoop(void) {
	MikanDraw->ClearScreen();
	switch (game)
	{
	case Game::TITLE:
		if (gameInit() == false) {return 1;}
		drawTitle();
		break;

	case Game::NORMAL:
		normalUpdate();
		drawGame(Game::NORMAL);
		break;

	case Game::GATARI:
		gatariUpdate();
		drawGame(Game::GATARI);
		break;

	case Game::KYOTOU:
		kyotouUpdate();
		drawGame(Game::KYOTOU);
		break;

	case Game::TRAIN:
		trainUpdate();
		drawGame(Game::TRAIN);
		break;

	case Game::RESULT:
		resultUpdate();
		drawResult();
		break;

	default:
		goTitle();
		break;
	}
	if (MikanInput->GetKeyNum(K_ESC)) {
		return 1;
	}
	return 0;
}

void CleanUp(void) {
	player.allDelete();
	item.clear();
	block.clear();
}
