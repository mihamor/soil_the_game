#include "Workbench.hpp"


String Workbench::intToStr(int value) {
	std::string str = std::to_string(value);
	String result(str);
	return result;
}

Workbench::Workbench(Inventory * inv, std::list<AbstractBlock*> &list, std::string fileName) {
	this->inv = inv;
	load(list, fileName);
};
Workbench::~Workbench() {
	for (Recipe r : recepies) {
		for (Slot * req : r.getRequired()) {
			delete req;
		}
		delete r.getResult();
	}
}

void Workbench::draw(RenderWindow & window, int vmodey, int vmodex, bool * isGui, HudItems * items, SoundSystem & soundSystem){


	/*RectangleShape background(Vector2f(vmodex - vmodex / 2, vmodey - vmodey / 2));
	background.setPosition(64, 64);*/
	auto background = items->craftBg;
	background.setPosition(64, 64);

	std::vector<RectangleShape> results;
	Text  amountShow;   //amount of slot
	Font font;
	if (!font.loadFromFile("fonts/arial.ttf")) assert(0);
	amountShow.setFont(font);
	amountShow.setFillColor(Color::White);
	amountShow.setCharacterSize(16);

	window.draw(background);
	int posy = 80;
	for (Recipe r : recepies) {

		int posx = 80;
		if (r.ifMatchesInv(*inv)) {
			for (Slot * s : r.getRequired()) {
				RectangleShape slotToDraw = *(s->block->getRectangleShape()); // background of slot
				
				slotToDraw.setPosition((float)posx, (float)posy);
				amountShow.setString(intToStr(s->amount));
				amountShow.setPosition(posx, posy);
				window.draw(slotToDraw);
				window.draw(amountShow);
				posx = posx + 2* BLOCK_SIZE;
				
			}
			posx = posx + BLOCK_SIZE;

			Slot * result = r.getResult();
			RectangleShape resultSlot = *(result->block->getRectangleShape()); // background of slot
			results.push_back(resultSlot);
			posx = background.getSize().x / 2 + 4*BLOCK_SIZE;

			resultSlot.setPosition(posx, posy);
			amountShow.setString(intToStr(result->amount));
			amountShow.setPosition(posx, posy);


			posy = posy + BLOCK_SIZE;
			bool added = false;
			if (isRectClicked(resultSlot, window) && !inv->isFull(result->block))
			{
				soundSystem.play("craft");
				added = true;
				std::cout << result->amount << std::endl;
				int amount = result->amount;
				for (int i = 0; i < amount; i++) inv->addSlot(result->block);
				for (Slot * req : r.getRequired()) for (int j = 0; j < req->amount; j++) inv->reduceAmount(*req->block);
				resultSlot.setFillColor(Color::Green);
			}
			else if (isRectClicked(resultSlot, window)) resultSlot.setFillColor(Color::Red);


			window.draw(resultSlot);
			window.draw(amountShow);
			if (added) sleep(milliseconds(200));


		}

		/*Event event;
		while (window.pollEvent(event)) {
			if (event.type == Event::KeyPressed && event.key.code == Keyboard::C) {
				*isGui = false; break;
			}
		}*/ if (!*isGui) return;
	}

}
void Workbench::load(std::list<AbstractBlock*> &blocks, std::string fileName) {
	TiXmlDocument doc(fileName.c_str());
	if (!doc.LoadFile()) abort(); //@todo error handling


	TiXmlElement * list = doc.FirstChildElement("recipes_list");

	TiXmlElement *recipe = list->FirstChildElement("recipe");
	while (recipe) {

		TiXmlElement *required = recipe->FirstChildElement("required");
		TiXmlElement *slotReq = required->FirstChildElement("slot");

		std::vector<Slot *> req;
		while (slotReq) {
			char signature = atoi(slotReq->Attribute("signature"));
			int amount = atoi(slotReq->Attribute("amount"));
			Slot * requ = new Slot(AbstractBlock::getBlockFromList(signature, blocks), amount);

			req.push_back(requ);
			slotReq = slotReq->NextSiblingElement("slot");
		}


		TiXmlElement *slotRes = recipe->FirstChildElement("result");
		char signature = atoi(slotRes->Attribute("signature"));
		int amount = atoi(slotRes->Attribute("amount"));
		Slot * res = new Slot(AbstractBlock::getBlockFromList(signature, blocks), amount);

		Recipe rec(req, res);
		recepies.push_back(rec);
		recipe = recipe->NextSiblingElement("recipe");
	}
}
void Workbench::workbenchSave(std::string fileName) {
	TiXmlDocument doc;
	TiXmlDeclaration * decl = new TiXmlDeclaration("1.0", "", "");
	doc.LinkEndChild(decl);

	TiXmlElement * list = new TiXmlElement("recipes_list");
	doc.LinkEndChild(list);

	for (Recipe r : recepies) {
		TiXmlElement * element = new TiXmlElement("recipe");
		TiXmlElement * required = new TiXmlElement("required");
		for (Slot *s : r.getRequired()) {
			TiXmlElement * slot = new TiXmlElement("slot");
			slot->SetAttribute("signature", s->block->getSingature());
			slot->SetAttribute("amount", s->amount);
			required->LinkEndChild(slot);
		}
		TiXmlElement * result = new TiXmlElement("result");
		Slot * res = r.getResult();
		result->SetAttribute("signature", res->block->getSingature());
		result->SetAttribute("amount", res->amount);



		element->LinkEndChild(required);
		element->LinkEndChild(result);
		list->LinkEndChild(element);
	}

	doc.SaveFile(fileName.c_str());
}
bool  Workbench::isRectClicked(sf::RectangleShape &rectToClick, sf::RenderWindow &window) {
	Vector2i mpos_i = Mouse::getPosition(window);
	Vector2f mpos_f = window.mapPixelToCoords(mpos_i);
	Vector2i a = Vector2i(mpos_f.x, mpos_f.y);

	sf::IntRect rect(rectToClick.getPosition().x, rectToClick.getPosition().y, rectToClick.getGlobalBounds().width,
		rectToClick.getGlobalBounds().height);
	if (rect.contains(a) && (sf::Mouse::isButtonPressed(sf::Mouse::Left))) {
		return true;
	}
	return false;
}