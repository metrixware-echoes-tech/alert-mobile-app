/*
 * AlertTab.cpp
 *
 *  Created on: 13 mai 2013
 *      Author: gdr
 */

#include "AlertTab.h"
/**
 * Constructor.
 */
AlertTab::AlertTab(int language, String loginToken) :
		Screen(), LANGUAGE(language), _LOGINTOKEN(loginToken) {
	// Set title and icon of the stack screen.
	setTitle(Convert::tr(CREATE_ALERT_TAB_EN + LANGUAGE));
	selectedAsset = -1;
	selectedPlugin = -1;
	selectedInformation = -1;
	selectedUnit = -1;
	selectedOperator = -1;
	selectedUser = -1;
	selectedMedia = -1;
	selectedMediaValue = -1;
	posOptionAlert = -1;
//			setBackgroundColor(255,255,255);
//			httpConnect = new HTTPConnect();
//			httpConnect->connectUrl("https://alert-api.echoes-tech.com/plugins/1/sources/1/searches/?login=thomas.saquet@echoes-tech.com&password=147258369aA", "parseJSONPlugin"/*,this*/);
	createUI();
}

/**
 * Destructor.
 */
AlertTab::~AlertTab() {

}

void AlertTab::handleKeyPress(int keyCode) {
	lprintfln("handleKeyPress : %d", keyCode);
	if (keyCode == MAK_BACK) {
		if (activeMainLayout == mainLayoutAssetChoice) {
			activeMainLayout = mainLayoutAlertChoice;
			Screen::setMainWidget(mainLayoutAlertChoice);
		} else if (activeMainLayout == mainLayoutPluginChoice) {
			activeMainLayout = mainLayoutAssetChoice;
			Screen::setMainWidget(mainLayoutAssetChoice);
		} else if (activeMainLayout == mainLayoutInfoChoice) {
			activeMainLayout = mainLayoutPluginChoice;
			Screen::setMainWidget(mainLayoutPluginChoice);
		} else if (activeMainLayout == mainLayoutOptionChoice) {
			activeMainLayout = mainLayoutInfoChoice;
			Screen::setMainWidget(mainLayoutInfoChoice);
		} else if (activeMainLayout == mainLayoutOperatorChoice
				|| activeMainLayout == mainLayoutUnitChoice) {
			activeMainLayout = mainLayoutOptionChoice;
			Screen::setMainWidget(mainLayoutOptionChoice);
		} else if (activeMainLayout == mainLayoutMediaChoice) {
			activeMainLayout = mainLayoutUserChoice;
			Screen::setMainWidget(mainLayoutUserChoice);
		} else if (activeMainLayout == mainLayoutMediaValueChoice) {
			activeMainLayout = mainLayoutMediaChoice;
			Screen::setMainWidget(mainLayoutMediaChoice);
		} else if (activeMainLayout == mainLayoutSnoozeChoice) {
			activeMainLayout = mainLayoutMediaValueChoice;
			Screen::setMainWidget(mainLayoutMediaValueChoice);
		} else if (activeMainLayout == mainLayoutAlertDetailChoice) {
			activeMainLayout = mainLayoutAlertChoice;
			Screen::setMainWidget(mainLayoutAlertChoice);
		} else if (activeMainLayout == mainLayoutAlertChoice) {
			maAlert("", "",
					Convert::tr(Screen_Main_Button_close_app + LANGUAGE),
					Convert::tr(Screen_Main_Button_home + LANGUAGE),
					Convert::tr(Screen_Main_Button_cancel + LANGUAGE));
		}
		activeMainLayout->setHeight(30000);
		activeMainLayout->fillSpaceVertically();
		activeMainLayout->fillSpaceHorizontally();
	}
}

void AlertTab::connectUrl(String url, eAlertTab fct, int verb,
		String jsonMessage) {
	lprintfln("connectUrl");

	if (mIsConnected == false) {
		mIsConnected = true;
		fonction = fct;
		lprintfln(url.c_str());
		if (verb == GET) {
			lprintfln("GET");
			int tmp = this->get(url.c_str());
			lprintfln("GET send = %d", tmp);
		} else if (verb == POST) {
			lprintfln("POST");
			lprintfln(jsonMessage.c_str());
			int tmp = this->postJsonRequest(url.c_str(), jsonMessage.c_str());
			lprintfln("POST send = %d", tmp);
		}
	} else {
		lprintfln("Déjà connecté: %d", fonction);
	}
}

void AlertTab::dataDownloaded(MAHandle data, int result) {
	this->close();
	lprintfln("dataDownloaded");
	mIsConnected = false;
	String sMessage = "";
	if (result == RES_OK) {
		connERR = 0;
		char * jsonData = new char[maGetDataSize(data) + 1];
		maReadData(data, jsonData, 0, maGetDataSize(data));
		String jsonTmp = jsonData;
		Convert::formatJSONBeforeParse(jsonTmp);

		MAUtil::YAJLDom::Value *root = YAJLDom::parse(
				(const unsigned char*) jsonTmp.c_str(), maGetDataSize(data));
		switch (fonction) {
		case PLUGIN_LIST:
			parseJSONPlugin(root);
			break;
		case INFORMATION_LIST:
			parseJSONInformation(root);
			break;
		case ALERT_LIST:
			parseJSONAlert(root);
			break;
		case ASSET_LIST:
			parseJSONAsset(root);
			break;
		case OPERATOR_LIST:
			parseJSONOperator(root);
			break;
		case UNIT_INFO:
			parseJSONUnitInfo(root);
			break;
		case SUB_UNIT_INFO:
			parseJSONSubUnitInfo(root);
			break;
		case USER_LIST:
			parseJSONUserForOrganisation(root);
			break;
		case MEDIA_TYPE:
			parseJSONMediaType(root);
			break;
		case MEDIA_VALUE_LIST:
			parseJSONMediaValue(root);
			break;
		case POST_MEDIA_SPE:
			parseJSONPostMediaSpe(root);
			break;
		case POST_ALERT:
			parseJSONPostAlert(root);
			break;
		case SEARCH_INFO:
			parseJSONSearchInfo(root);
			break;
		case ALERT_INFO:
			parseJSONAlertInfo(root);
			break;
		case ALERT_RECIPIENT_LIST:
			parseJSONAlertRecipientList(root);
			break;
		case UNITS_TYPES:
			parseJSONUnitsTypes(root);
		default:
			break;

			delete jsonData;
			delete root;
		}
	} else if (result == CONNERR_DNS) {
		connERR++;
		lprintfln("AlertTab DataDownload result = %d", result);
		lprintfln("DNS resolution error.");
	} else if (result == 404 && fonction == ALERT_LIST ) {
		lListTitle->setText(Convert::tr(alert_list_Label_title_no_alert + LANGUAGE));
	}
	else {
		connERR++;
		lprintfln("AlertTab DataDownload result = %d", result);
	}
	if (connERR >= 3) {
		sMessage = "Connection Error. ERREUR :";
		sMessage += Convert::toString(result);
		maMessageBox("Connection Error", sMessage.c_str());
		getSystemConnection();
	}
}

void AlertTab::parseJSONAlertRecipientList(MAUtil::YAJLDom::Value* root) {
	lprintfln("parseJSONAlertRecipientList");
	int tmpSize = mapLVIRecipients.size();
	for(int idx1 = 0; idx1 < tmpSize ; idx1++)
	{
		lVRecipients->removeChild(mapLVIRecipients[idx1]);
		mapLVIRecipients[idx1]->removeChild(mapLRecipients[idx1]);
		delete mapLRecipients[idx1];
		delete mapLVIRecipients[idx1];
	}
	mapLRecipients.clear();
	mapLVIRecipients.clear();

	if (NULL == root || YAJLDom::Value::NUL == root->getType()
			|| YAJLDom::Value::ARRAY != root->getType()) {
		lprintfln("Root node is not valid\n");
	} else {
		lprintfln("Root node is valid :) \n");
		for (int idx = 0; idx <= root->getNumChildValues() - 1; idx++) {
			MAUtil::YAJLDom::Value* valueTmp = root->getValueByIndex(idx);
			MAUtil::YAJLDom::Value* valueTmp1 = valueTmp->getValueForKey("media_value");

			mapLVIRecipients[idx] = new ListViewItem();
			lVRecipients->addChild(mapLVIRecipients[idx]);
			mapLRecipients[idx] = new Label(valueTmp1->getValueForKey("value")->toString());
			mapLVIRecipients[idx]->addChild(mapLRecipients[idx]);
		}

		Screen::setMainWidget(mainLayoutAlertDetailChoice);
		activeMainLayout = mainLayoutAlertDetailChoice;
	}
}

void AlertTab::parseJSONUnitsTypes(MAUtil::YAJLDom::Value* root){
	lprintfln("parseJSONUnitsTypes");
		if (NULL == root || YAJLDom::Value::NUL == root->getType()
				|| YAJLDom::Value::ARRAY == root->getType()) {
			lprintfln("Root node is not valid\n");
		} else {
			lprintfln("Root node is valid :) \n");
	//		MAUtil::YAJLDom::Value* valueTmp = root->getValueByIndex(0);
	//		MAUtil::YAJLDom::Value* valueTmp1 = root->getValueForKey("id");
			if (root->getValueForKey("name")->toString() == "number") {
				eBValue->setInputMode(EDIT_BOX_INPUT_MODE_NUMERIC);
			} else {
				eBValue->setInputMode(EDIT_BOX_INPUT_MODE_ANY);
			}
			Screen::setMainWidget(mainLayoutOptionChoice);
			activeMainLayout = mainLayoutOptionChoice;
		}
}

void AlertTab::parseJSONSearchInfo(MAUtil::YAJLDom::Value* root) {
	lprintfln("parseJSONSearchInfo");
	if (NULL == root || YAJLDom::Value::NUL == root->getType()
			|| YAJLDom::Value::ARRAY == root->getType()) {
		lprintfln("Root node is not valid\n");
	} else {
		lprintfln("Root node is valid :) \n");
//		MAUtil::YAJLDom::Value* valueTmp = root->getValueByIndex(0);
//		MAUtil::YAJLDom::Value* valueTmp1 = root->getValueForKey("id");
		if (root->getValueForKey("pos_key_value")->toInt() == 0) {
			lprintfln("pasdispo");
			eBKeyValue->setText("N/A");
			eBKeyValue->setEnabled(false);
		} else {
			lprintfln("dispo");
			eBKeyValue->setText("");
			eBKeyValue->setEnabled(true);
		}
		String urlTmp = HOST;
		urlTmp += "/units/" + Convert::toString(mapInfoIdUnit[selectedInformation]) + "/types/";
		urlTmp += _LOGINTOKEN;
		connectUrl(urlTmp, UNITS_TYPES);
	}
}

void AlertTab::parseJSONPostAlert(MAUtil::YAJLDom::Value* root) {
	lprintfln("parseJSONPostAlert");
	String urlTmp = HOST;
	urlTmp += "/alerts/";
	urlTmp += _LOGINTOKEN;
	connectUrl(urlTmp, ALERT_LIST);

	int index = mapSnoozeList.size();
	for (int idx = 0; idx < index; idx++) {
		lVListDest->removeChild(mapLVIListDest[idx]);
		mapLVIListDest[idx]->removeChild(mapLListDestName[idx]);
		delete mapLListDestName[idx];
		delete mapLVIListDest[idx];
	}
	mapLListDestName.clear();
	mapLVIListDest.clear();
	mapAMSId.clear();
	mapMediaValueListId.clear();
	mapSnoozeList.clear();
	selectedAsset = -1;
	selectedPlugin = -1;
	selectedInformation = -1;
	selectedUnit = -1;
	selectedOperator = -1;
	selectedUser = -1;
	selectedMedia = -1;
	selectedMediaValue = -1;
}

void AlertTab::parseJSONAlertInfo(MAUtil::YAJLDom::Value* root) {
	lprintfln("parseJSONAlertInfo");
	if (NULL == root || YAJLDom::Value::NUL == root->getType()
			|| YAJLDom::Value::ARRAY == root->getType()) {
		lprintfln("Root node is not valid\n");
	} else {
		lprintfln("Root node is valid :) \n");
//		MAUtil::YAJLDom::Value* valueTmp = root->getValueByIndex(0);
		MAUtil::YAJLDom::Value* valueTmp1 = root->getValueForKey("alert");

		lAlertName->setText(
				Convert::tr(alert_create_String_Alert_Name + LANGUAGE)
						+ valueTmp1->getValueForKey("name")->toString());
		MAUtil::YAJLDom::Value* valueTmp2 = root->getValueForKey(
				"criteria");
		lAlertOperator->setText(
				Convert::tr(alert_create_String_Operator_Name + LANGUAGE)
						+ valueTmp2->getValueForKey("name")->toString());
		MAUtil::YAJLDom::Value* valueTmp3 = root->getValueForKey(
				"alert_value");
		lAlertValue->setText(
				Convert::tr(alert_create_String_Threshold_Value + LANGUAGE)
						+ valueTmp3->getValueForKey("value")->toString());
		MAUtil::YAJLDom::Value* valueTmp4 = root->getValueForKey(
				"information_unit");
		lAlertUnit->setText(
				Convert::tr(alert_create_String_Unit + LANGUAGE)
						+ valueTmp4->getValueForKey("name")->toString());
		lAlertKeyValue->setText(
				Convert::tr(alert_create_String_Key_Value + LANGUAGE)
						+ valueTmp3->getValueForKey("key_value")->toString());
//					lAlertMedia->setText("Média : " + valueTmp15->getValueForKey("key_value")->toString());
//					lAlertSnooze->setText("Snooze : " + valueTmp15->getValueForKey("key_value")->toString());

//		Screen::setMainWidget(mainLayoutAlertDetailChoice);
//			activeMainLayout = mainLayoutAlertDetailChoice;
		String urlTmp = HOST;
		urlTmp += "/alerts/" + Convert::toString(mapAlertId[posOptionAlert]) + "/recipients/";
		urlTmp += _LOGINTOKEN;
		connectUrl(urlTmp, ALERT_RECIPIENT_LIST);
	}
}

void AlertTab::parseJSONPostMediaSpe(MAUtil::YAJLDom::Value* root) {
	lprintfln("parseJSONPostMediaSpe");
	if (NULL == root || YAJLDom::Value::NUL == root->getType()
			|| YAJLDom::Value::ARRAY == root->getType()) {
		lprintfln("Root node is not valid\n");
	} else {
		lprintfln("ID AMS = %d", root->getValueForKey("id")->toInt());
		mapAMSId[mapAMSId.size()] = root->getValueForKey("id")->toInt();
	}
	currentAMS++;
//		lprintfln("currentAMS = %d", currentAMS);
//lprintfln("mapMediaValueListId.size() = %d", mapMediaValueListId.size());
	if (currentAMS < mapMediaValueListId.size()) {
		String urlTmp = HOST;
		urlTmp += "/medias/specializations/";
		urlTmp += _LOGINTOKEN;
		String message = "{\"ams_snooze\": ";
		message += Convert::toString(mapSnoozeList[currentAMS]);
//				lprintfln(" mapSnoozeList[currentAMS] = %d",  mapSnoozeList[currentAMS]);
		message += ",\"mev_id\" : ";
		message += Convert::toString(mapMediaValueListId[currentAMS]);
//				lprintfln(" mapMediaValueListId[currentAMS] = %d",  mapMediaValueListId[currentAMS]);
		message += "}";
		connectUrl(urlTmp, POST_MEDIA_SPE, POST, message);
	} else {
		String urlTmp = HOST;
		urlTmp += "/alerts/";
		urlTmp += _LOGINTOKEN;

		String message = "{\"name\": \""
				+ mapLAssetName[selectedAsset]->getText() + " - "
				+ mapLPluginName[selectedPlugin]->getText() + " - "
				+ mapLInfoName[selectedInformation]->getText()
				+ "\",\
\"alert_value\": \"" + eBValue->getText()
				+ "\",\
\"thread_sleep\": 0,\
\"key_value\": \""
				+ eBKeyValue->getText() + "\",\
\"ast_id\": "
				+ Convert::toString(mapAssetId[selectedAsset])
				+ ",\
\"sea_id\": "
				+ Convert::toString(mapInfoIdSea[selectedInformation])
				+ ",\
\"src_id\": "
				+ Convert::toString(mapInfoIdSrc[selectedInformation])
				+ ",\
\"plg_id\": "
				+ Convert::toString(mapPluginId[selectedPlugin])
				+ ",\
\"inf_value_num\": "
				+ Convert::toString(mapInfoInfValueNum[selectedInformation])
				+ ",\
\"inu_id\": "
				+ Convert::toString(mapInfoIdUnit[selectedInformation])
				+ ",\
\"acr_id\": "
				+ Convert::toString(mapOperatorId[selectedOperator])
				+ ",\
\"ams_id\": [";
		for (int idx = 0; idx < mapAMSId.size(); idx++) {
			if (idx != 0) {
				message += ", ";
			}
			message += Convert::toString(mapAMSId[idx]);
		}
		message += "]\
				 }";
		printf(message.c_str());
		connectUrl(urlTmp, POST_ALERT, POST, message);
	}
}

void AlertTab::parseJSONMediaValue(MAUtil::YAJLDom::Value* root) {
	lprintfln("parseJSONMediaValue");
	if (NULL == root || YAJLDom::Value::NUL == root->getType()
			|| YAJLDom::Value::ARRAY != root->getType()) {
		lprintfln("Root node is not valid\n");
	} else {
		lprintfln("Root node is valid :) \n");

		if(mainLayoutMediaValueChoice == NULL)
		{
			mainLayoutMediaValueChoice = new VerticalLayout();
			// Make the layout fill the entire screen.
			mainLayoutMediaValueChoice->fillSpaceHorizontally();
			mainLayoutMediaValueChoice->fillSpaceVertically();

			mediaValueTitle = new Label(Convert::tr(alert_create_Label_media_value_choice + LANGUAGE));
			mediaValueTitle->fillSpaceHorizontally();
			mainLayoutMediaValueChoice->addChild(mediaValueTitle);
			lVMediaValue = new ListView();
			lVMediaValue->addListViewListener(this);
			mainLayoutMediaValueChoice->addChild(lVMediaValue);
		}
		else
		{
			int index = mapLVIMediaValue.size();
			for (int idx = 0; idx < index; idx++) {
					lVMediaValue->removeChild(mapLVIMediaValue[idx]);
					mapLVIMediaValue[idx]->removeChild(mapLMediaValueName[idx]);
					delete mapLMediaValueName[idx];
					delete mapLVIMediaValue[idx];
				}
			mapLMediaValueName.clear();
			mapLVIMediaValue.clear();
		}

		for (int idx = 0; idx <= root->getNumChildValues() - 1; idx++) {
			MAUtil::YAJLDom::Value* valueTmp = root->getValueByIndex(idx);
			mapMediaValueId[idx] = valueTmp->getValueForKey("id")->toInt();
			mapLMediaValueName[idx] = new Label();
			mapLMediaValueName[idx]->setText(
					(valueTmp->getValueForKey("value")->toString().c_str()));
			mapLVIMediaValue[idx] = new ListViewItem();
			mapLVIMediaValue[idx]->addChild(mapLMediaValueName[idx]);
			lVMediaValue->addChild(mapLVIMediaValue[idx]);

		}
	}
	Screen::setMainWidget(mainLayoutMediaValueChoice);
	activeMainLayout = mainLayoutMediaValueChoice;
}

void AlertTab::parseJSONMediaType(MAUtil::YAJLDom::Value* root) {
	lprintfln("parseJSONMediaType");
	if (NULL == root || YAJLDom::Value::NUL == root->getType()
			|| YAJLDom::Value::ARRAY != root->getType()) {
		lprintfln("Root node is not valid\n");
	} else {
		lprintfln("Root node is valid :) \n");
		if(mainLayoutMediaChoice == NULL)
		{
			mainLayoutMediaChoice = new VerticalLayout();
			// Make the layout fill the entire screen.
			mainLayoutMediaChoice->fillSpaceHorizontally();
			mainLayoutMediaChoice->fillSpaceVertically();

			mediaTitle = new Label(Convert::tr(alert_create_Label_media_choice + LANGUAGE));
			mediaTitle->fillSpaceHorizontally();
			mainLayoutMediaChoice->addChild(mediaTitle);
			lVMedia = new ListView();
			lVMedia->addListViewListener(this);
			mainLayoutMediaChoice->addChild(lVMedia);
		}
		else
		{
			int index = mapLVIMedia.size();
			for (int idx = 0; idx < index; idx++) {
				lVMedia->removeChild(mapLVIMedia[idx]);
				mapLVIMedia[idx]->removeChild(mapLMediaName[idx]);
				delete mapLMediaName[idx];
				delete mapLVIMedia[idx];
			}
			mapLMediaName.clear();
			mapLVIMedia.clear();
		}
		for (int idx = 0; idx <= root->getNumChildValues() - 1; idx++) {
			MAUtil::YAJLDom::Value* valueTmp = root->getValueByIndex(idx);
			mapMediaId[idx] = valueTmp->getValueForKey("id")->toInt();
			mapLMediaName[idx] = new Label();
			mapLMediaName[idx]->setText(
					(valueTmp->getValueForKey("name")->toString().c_str()));
			mapLVIMedia[idx] = new ListViewItem();
			mapLVIMedia[idx]->addChild(mapLMediaName[idx]);
			lVMedia->addChild(mapLVIMedia[idx]);

		}
	}

	Screen::setMainWidget(mainLayoutMediaChoice);
	activeMainLayout = mainLayoutMediaChoice;
}

void AlertTab::parseJSONUserForOrganisation(MAUtil::YAJLDom::Value* root) {
	lprintfln("parseJSONSubUnitInfo");

	if (NULL == root || YAJLDom::Value::NUL == root->getType()
			|| YAJLDom::Value::ARRAY != root->getType()) {
		lprintfln("Root node is not valid\n");
	} else {
		lprintfln("Root node is valid :) \n");
		if(mainLayoutUserChoice == NULL)
		{
		mainLayoutUserChoice = new VerticalLayout();
		// Make the layout fill the entire screen.
		mainLayoutUserChoice->fillSpaceHorizontally();
		mainLayoutUserChoice->fillSpaceVertically();
		userTitle = new Label(Convert::tr(alert_create_Label_user_choice + LANGUAGE));
		userTitle->fillSpaceHorizontally();
			mainLayoutUserChoice->addChild(userTitle);
		lVUser = new ListView();
		lVUser->addListViewListener(this);
		mainLayoutUserChoice->addChild(lVUser);
		}
		else {
			int index = mapLVIUser.size();
			for (int idx = 0; idx < index; idx++) {
				lVUser->removeChild(mapLVIUser[idx]);
				mapLVIUser[idx]->removeChild(mapLUserName[idx]);
				delete mapLUserName[idx];
				delete mapLVIUser[idx];
			}
			mapLUserName.clear();
			mapLVIUser.clear();
		}

		for (int idx = 0; idx <= root->getNumChildValues() - 1; idx++) {
			MAUtil::YAJLDom::Value* valueTmp = root->getValueByIndex(idx);
			mapUserId[idx] = valueTmp->getValueForKey("id")->toInt();
			mapLUserName[idx] = new Label();
			String space = " ";
			mapLUserName[idx]->setText(
					(valueTmp->getValueForKey("first_name")->toString() + space
							+ valueTmp->getValueForKey("last_name")->toString()).c_str());
			mapLVIUser[idx] = new ListViewItem();
			mapLVIUser[idx]->addChild(mapLUserName[idx]);
			lVUser->addChild(mapLVIUser[idx]);

		}
	}
	Screen::setMainWidget(mainLayoutUserChoice);
	activeMainLayout = mainLayoutUserChoice;
}

void AlertTab::parseJSONSubUnitInfo(MAUtil::YAJLDom::Value* root) {
	lprintfln("parseJSONSubUnitInfo");

	if (NULL == root || YAJLDom::Value::NUL == root->getType()
			|| YAJLDom::Value::ARRAY != root->getType()) {
		lprintfln("Root node is not valid\n");
	} else {
		lprintfln("Root node is valid :) \n");

		int index = mapLVIUser.size();

		for (int idx = 0; idx <= root->getNumChildValues() - 1; idx++) {
			MAUtil::YAJLDom::Value* valueTmp = root->getValueByIndex(idx);
			mapLUnitName[idx + 1] = new Label();
			mapLUnitName[idx + 1]->setText(
					valueTmp->getValueForKey("name")->toString());
			mapUnitId[idx + 1] = valueTmp->getValueForKey("id")->toInt();
		}
		createUnitPage();
	}

}

void AlertTab::parseJSONUnitInfo(MAUtil::YAJLDom::Value* root) {
	lprintfln("parseJSONUnitInfo");

	if (NULL == root || YAJLDom::Value::NUL == root->getType()
			|| YAJLDom::Value::ARRAY == root->getType()) {
		lprintfln("Root node is not valid\n");
	} else {
		lprintfln("Root node is valid :) \n");

		int index = mapLVIUnit.size();
		for (int idx = 0; idx < index; idx++) {
			lVUnit->removeChild(mapLVIUnit[idx]);
			mapLVIUnit[idx]->removeChild(mapLUnitName[idx]);
			delete mapLUnitName[idx];
			delete mapLVIUnit[idx];
		}
		mapLUnitName.clear();
		mapLVIUnit.clear();

		mapLUnitName[0] = new Label();
		mapLUnitName[0]->setText(root->getValueForKey("name")->toString());
		String UnitTmp = "Unit : ";
		UnitTmp += mapLUnitName[0]->getText();
		unit->setText(UnitTmp);
		nbOfSubUnits = root->getValueForKey("information_sub_units")->toInt();

//		TODO :
		String urlTmp = HOST;
		urlTmp += "/plugins/" + Convert::toString(mapPluginId[selectedPlugin])
				+ "/sources/"
				+ Convert::toString(mapInfoIdSrc[selectedInformation])
				+ "/searches/"
				+ Convert::toString(mapInfoIdSea[selectedInformation]);
		urlTmp += _LOGINTOKEN;
		connectUrl(urlTmp, SEARCH_INFO);
	}
}

void AlertTab::parseJSONOperator(MAUtil::YAJLDom::Value* root) {
	lprintfln("parseJSONOperator");

	if (NULL == root || YAJLDom::Value::NUL == root->getType()
			|| YAJLDom::Value::ARRAY != root->getType()) {
		lprintfln("Root node is not valid\n");
	} else {
		lprintfln("Root node is valid :) \n");
		if(mainLayoutOperatorChoice == NULL)
		{
		mainLayoutOperatorChoice = new VerticalLayout();
		// Make the layout fill the entire screen.
		mainLayoutOperatorChoice->fillSpaceHorizontally();
		mainLayoutOperatorChoice->fillSpaceVertically();
		operatorTitle = new Label(Convert::tr(alert_create_Label_operator_choice + LANGUAGE));
		mainLayoutOperatorChoice->addChild(operatorTitle);
//		Screen::setMainWidget(mainLayoutOperatorChoice);
//		activeMainLayout = mainLayoutOperatorChoice;
		lVOperator = new ListView();
		lVOperator->addListViewListener(this);
		mainLayoutOperatorChoice->addChild(lVOperator);
		}
		else
		{
			int index = mapLVIOperator.size();
			for (int idx = 0; idx < index; idx++) {
				lVOperator->removeChild(mapLVIOperator[idx]);
				mapLVIOperator[idx]->removeChild(mapLOperatorName[idx]);
				delete mapLOperatorName[idx];
				delete mapLVIOperator[idx];
			}
			mapLOperatorName.clear();
			mapLVIOperator.clear();
		}
	}
	for (int idx = 0; idx <= root->getNumChildValues() - 1; idx++) {
		MAUtil::YAJLDom::Value* valueTmp = root->getValueByIndex(idx);
		mapOperatorId[idx] = valueTmp->getValueForKey("id")->toInt();
		mapLOperatorName[idx] = new Label();
		mapLOperatorName[idx]->setText(
				valueTmp->getValueForKey("name")->toString().c_str());
		mapLVIOperator[idx] = new ListViewItem();
		mapLVIOperator[idx]->addChild(mapLOperatorName[idx]);
		lVOperator->addChild(mapLVIOperator[idx]);

	}
	Screen::setMainWidget(mainLayoutOperatorChoice);
	activeMainLayout = mainLayoutOperatorChoice;

}

void AlertTab::parseJSONAsset(MAUtil::YAJLDom::Value* root) {
	lprintfln("parseJSONAsset");

	if (NULL == root || YAJLDom::Value::NUL == root->getType()
			|| YAJLDom::Value::ARRAY != root->getType()) {
		lprintfln("Root node is not valid\n");
	} else {
		lprintfln("Root node is valid :) \n");
		if(mainLayoutAssetChoice == NULL)
		{
		mainLayoutAssetChoice = new VerticalLayout();
		mainLayoutAssetChoice->fillSpaceHorizontally();

		assetTitle = new Label();
		assetTitle->fillSpaceHorizontally();
		assetTitle->setText(
				Convert::tr(alert_create_Label_asset_choice + LANGUAGE));
		mainLayoutAssetChoice->addChild(assetTitle);
		lVAsset = new ListView();
		lVAsset->addListViewListener(this);
		mainLayoutAssetChoice->addChild(lVAsset);
		}
		else{
			int index = mapLVIAsset.size();
			for (int idx = 0; idx < index; idx++) {
				lVAsset->removeChild(mapLVIAsset[idx]);
				mapLVIAsset[idx]->removeChild(mapLAssetName[idx]);
				delete mapLAssetName[idx];
				delete mapLVIAsset[idx];
			}
			mapLAssetName.clear();
			mapLVIAsset.clear();
		}
	}
	for (int idx = 0; idx <= root->getNumChildValues() - 1; idx++) {
//				lprintfln("dans le for Asset");
		MAUtil::YAJLDom::Value* valueTmp = root->getValueByIndex(idx);
//				mapAssetName[idx] = valueTmp->getValueForKey("name")->toString();
		mapAssetId[idx] = valueTmp->getValueForKey("id")->toInt();
//				lprintfln(mapAssetName[idx].c_str());
		mapLAssetName[idx] = new Label();
		mapLAssetName[idx]->setText(
				valueTmp->getValueForKey("name")->toString().c_str());
//				mapLAssetName[idx]->setText(mapAssetName[idx].c_str());
		mapLVIAsset[idx] = new ListViewItem();
		mapLVIAsset[idx]->addChild(mapLAssetName[idx]);
		lVAsset->addChild(mapLVIAsset[idx]);

	}
	mainLayoutAssetChoice->fillSpaceVertically();
	Screen::setMainWidget(mainLayoutAssetChoice);
	activeMainLayout = mainLayoutAssetChoice;
}

void AlertTab::parseJSONAlert(MAUtil::YAJLDom::Value* root) {
	lprintfln("parseJSONAlert");
	posOptionAlert = -1;

	for (int idx0 = 0; idx0 < mapAlertId.size(); idx0++) {
		lValert->removeChild(mapLVIAlert[idx0]);
	}
	mapAlertId.clear();

	if (NULL == root || YAJLDom::Value::NUL == root->getType()
			|| YAJLDom::Value::ARRAY != root->getType()) {
		lprintfln("Root node is not valid\n");

	} else {
		lprintfln("Root node is valid :) \n");
	}
	lListTitle->setText(Convert::tr(alert_list_Label_title + LANGUAGE));
	Screen::setMainWidget(mainLayoutAlertChoice);
	activeMainLayout = mainLayoutAlertChoice;
	for (int idx = 0; idx <= root->getNumChildValues() - 1; idx++) {
		MAUtil::YAJLDom::Value* valueTmp = root->getValueByIndex(idx);
		MAUtil::YAJLDom::Value* valueTmp1 = valueTmp->getValueForKey("alert");

		mapAlertId[idx] = valueTmp1->getValueForKey("id")->toInt();
		mapLAlertName[idx] = new Label();
		mapLAlertName[idx]->fillSpaceHorizontally();
//		mapLAlertName[idx]->setTextVerticalAlignment(MAW_ALIGNMENT_CENTER);
		mapLAlertName[idx]->setText(
				valueTmp1->getValueForKey("name")->toString().c_str());

		mapLVIAlert[idx] = new ListViewItem();
		mapLVIAlert[idx]->addChild(mapLAlertName[idx]);

		lValert->addChild(mapLVIAlert[idx]);
	}

}

void AlertTab::parseJSONInformation(MAUtil::YAJLDom::Value* root) {
	lprintfln("parseJSONInformation");

	if (NULL == root || YAJLDom::Value::NUL == root->getType()
			|| YAJLDom::Value::ARRAY != root->getType()) {
		lprintfln("Root node is not valid\n");

	} else {
		lprintfln("Root node is valid :) \n");
		if(mainLayoutInfoChoice == NULL){
		mainLayoutInfoChoice = new VerticalLayout();
		// Make the layout fill the entire screen.
		mainLayoutInfoChoice->fillSpaceHorizontally();
		mainLayoutInfoChoice->fillSpaceVertically();
		infoTitle = new Label(Convert::tr(alert_create_Label_info_choice + LANGUAGE));
		infoTitle->fillSpaceHorizontally();
		mainLayoutInfoChoice->addChild(infoTitle);
//		Screen::setMainWidget(mainLayoutInfoChoice);
//		activeMainLayout = mainLayoutInfoChoice;
		lVInfo = new ListView();
		lVInfo->addListViewListener(this);
		mainLayoutInfoChoice->addChild(lVInfo);
		}
		else {
			int index = mapLVIInfo.size();
			for (int idx = 0; idx < index; idx++) {
				lVInfo->removeChild(mapLVIInfo[idx]);
				mapLVIInfo[idx]->removeChild(mapLInfoName[idx]);
				delete mapLInfoName[idx];
				delete mapLVIInfo[idx];
			}
			mapLInfoName.clear();
			mapLVIInfo.clear();
		}

	lprintfln("%d\n", root->getNumChildValues());
	for (int idx = 0; idx <= root->getNumChildValues() - 1; idx++) {

		MAUtil::YAJLDom::Value* valueTmp = root->getValueByIndex(idx);
		if (valueTmp->getValueForKey("display")->toString() == "true") {
			MAUtil::YAJLDom::Value* valueTmp1 = valueTmp->getValueForKey("id");
			mapLInfoName[idx] = new Label();
			mapLInfoName[idx]->setText(
					valueTmp->getValueForKey("name")->toString().c_str());
			mapInfoIdSea[idx] = valueTmp1->getValueForKey("search_id")->toInt();
			mapInfoIdSrc[idx] = valueTmp1->getValueForKey("source_id")->toInt();
			mapInfoIdUnit[idx] = valueTmp1->getValueForKey("unit_id")->toInt();
			mapInfoInfValueNum[idx] = valueTmp1->getValueForKey(
					"sub_search_number")->toInt();
			mapLVIInfo[idx] = new ListViewItem();
			mapLVIInfo[idx]->addChild(mapLInfoName[idx]);
			lVInfo->addChild(mapLVIInfo[idx]);
		}
	}
	Screen::setMainWidget(mainLayoutInfoChoice);
	activeMainLayout = mainLayoutInfoChoice;
	}
}

void AlertTab::parseJSONPlugin(MAUtil::YAJLDom::Value* root) {

	if (NULL == root || YAJLDom::Value::NUL == root->getType()
			|| YAJLDom::Value::ARRAY != root->getType()) {
		lprintfln("Root node is not valid\n");

	} else {
		lprintfln("Root node is valid :) \n");
		if(mainLayoutPluginChoice == NULL)
		{
		mainLayoutPluginChoice = new VerticalLayout();
		mainLayoutPluginChoice->fillSpaceHorizontally();
		mainLayoutPluginChoice->fillSpaceVertically();
		pluginTitle = new Label(Convert::tr(alert_create_Label_plugin_choice + LANGUAGE));
		pluginTitle->fillSpaceHorizontally();
		mainLayoutPluginChoice->addChild(pluginTitle);
//		Screen::setMainWidget(mainLayoutPluginChoice);
//		activeMainLayout = mainLayoutPluginChoice;
		lVPlugin = new ListView();
		lVPlugin->addListViewListener(this);
		mainLayoutPluginChoice->addChild(lVPlugin);
		}
		else {
			int index = mapLVIPlugin.size();
			for (int idx = 0; idx < index; idx++) {
				lVPlugin->removeChild(mapLVIPlugin[idx]);
				mapLVIPlugin[idx]->removeChild(mapLPluginName[idx]);
				delete mapLPluginName[idx];
				delete mapLVIPlugin[idx];
			}
			mapLPluginName.clear();
			mapLVIPlugin.clear();
		}

	lprintfln("%d\n", root->getNumChildValues());
	for (int idx = 0; idx <= root->getNumChildValues() - 1; idx++) {
		MAUtil::YAJLDom::Value* valueTmp = root->getValueByIndex(idx);

		String test1 = valueTmp->getValueForKey("name")->toString();
//		mapPluginName[idx] = valueTmp->getValueForKey("name")->toString();
		mapPluginId[idx] = valueTmp->getValueForKey("id")->toInt();
		mapLPluginName[idx] = new Label();
		mapLPluginName[idx]->setText(
				valueTmp->getValueForKey("name")->toString().c_str());
		mapLVIPlugin[idx] = new ListViewItem();
		mapLVIPlugin[idx]->addChild(mapLPluginName[idx]);
		lVPlugin->addChild(mapLVIPlugin[idx]);
	}
	Screen::setMainWidget(mainLayoutPluginChoice);
	activeMainLayout = mainLayoutPluginChoice;
	}
}


void AlertTab::listViewItemClicked(ListView* listView,
		ListViewItem* listViewItem) {
	lprintfln("Clicked");
	if (listView == lValert) {
		if (posOptionAlert != -1) //savoir si on a ouvert les options (supprimer/annuler/view), d'une alerte dans la View 1
				{
			mapLVIAlert[posOptionAlert]->removeChild(HLOptionAlert);
			mapLVIAlert[posOptionAlert]->addChild(
					mapLAlertName[posOptionAlert]);
			posOptionAlert = -1;
		}
		for (int i = 0; i < mapLVIAlert.size(); i++) {
			if (mapLVIAlert[i] == listViewItem) {
				posOptionAlert = i;
				mapLVIAlert[i]->removeChild(mapLAlertName[i]);
				HLOptionAlert = new HorizontalLayout();

				bDelete = new Button();
				bDelete->addButtonListener(this);
				bDelete->setText(
						Convert::tr(
								alert_create_Button_Delete_Alert + LANGUAGE));
				bDelete->fillSpaceHorizontally();
				HLOptionAlert->addChild(bDelete);
				bCancel = new Button();
				bCancel->setText(
						Convert::tr(
								alert_create_Button_Cancel_Alert + LANGUAGE));
//			bCancel->fillSpaceHorizontally();
				bCancel->addButtonListener(this);
				HLOptionAlert->addChild(bCancel);
				bView = new Button();
				bView->addButtonListener(this);
				bView->setText(
						Convert::tr(
								alert_create_Button_Detail_Alert + LANGUAGE));
				bView->fillSpaceHorizontally();
				HLOptionAlert->addChild(bView);
				mapLVIAlert[i]->addChild(HLOptionAlert);
			}
		}
	} else if (listView == lVAsset) {
		for (int i = 0; i < mapLVIAsset.size(); i++) {
			if (mapLVIAsset[i] == listViewItem) {
				selectedAsset = i;
				String urlTmp = HOST;
				urlTmp += "/assets/";
				urlTmp += Convert::toString(mapAssetId[i]);
				urlTmp += "/plugins/";
				urlTmp += _LOGINTOKEN;
				lprintfln(urlTmp.c_str());
				connectUrl(urlTmp, PLUGIN_LIST);
			}
		}
	} else if (listView == lVPlugin) {
		for (int i = 0; i < mapLVIPlugin.size(); i++) {
			if (mapLVIPlugin[i] == listViewItem) {
				selectedPlugin = i;
				String urlTmp = HOST;
				urlTmp += "/plugins/";
				urlTmp += Convert::toString(mapPluginId[i]);
				urlTmp += "/informations/";
				urlTmp += _LOGINTOKEN;
				lprintfln(urlTmp.c_str());
				connectUrl(urlTmp, INFORMATION_LIST);
			}
		}
	} else if (listView == lVInfo) {

		for (int idx = 0; idx < mapInfoIdUnit.size(); idx++) {
			if (listViewItem == mapLVIInfo[idx]) {
				selectedInformation = idx;
				mapUnitId[0] = mapInfoIdUnit[idx];
			}
		}
		String urlTmp = HOST;
		urlTmp += "/units/";
		urlTmp += Convert::toString(mapUnitId[0]);
		urlTmp += _LOGINTOKEN;
		connectUrl(urlTmp, UNIT_INFO);
		createOptionPage();

	} else if (listViewItem == mapLVIOption[1]) {
		maWidgetSetProperty(eBKeyValue->getWidgetHandle(),MAW_EDIT_BOX_SHOW_KEYBOARD, "false");
		maWidgetSetProperty(eBValue->getWidgetHandle(),MAW_EDIT_BOX_SHOW_KEYBOARD, "false");
		String urlTmp = HOST;
		urlTmp += "/criteria/";
		urlTmp += _LOGINTOKEN;
		connectUrl(urlTmp, OPERATOR_LIST);
	} else if (listView == lVOperator) {
		String criterionTmp = Convert::tr(
				alert_create_String_Operator_Name + LANGUAGE);
		for (int idx = 0; idx < mapLVIOperator.size(); idx++) {
			if (mapLVIOperator[idx] == listViewItem) {
				selectedOperator = idx;
				criterionTmp += mapLOperatorName[idx]->getText();
			}
		}
		criterion->setText(criterionTmp);
		Screen::setMainWidget(mainLayoutOptionChoice);
		activeMainLayout = mainLayoutOptionChoice;
	} else if (listViewItem == mapLVIOption[0]) {
		maWidgetSetProperty(eBKeyValue->getWidgetHandle(),MAW_EDIT_BOX_SHOW_KEYBOARD, "false");
				maWidgetSetProperty(eBValue->getWidgetHandle(),MAW_EDIT_BOX_SHOW_KEYBOARD, "false");
		if (nbOfSubUnits != 0 && mapUnitId.size() == 1) {
			String urlTmp1 = HOST;
			urlTmp1 += "/units/";
			urlTmp1 += Convert::toString(mapUnitId[0]);
			urlTmp1 += "/subunits/";
			urlTmp1 += _LOGINTOKEN;
			connectUrl(urlTmp1, SUB_UNIT_INFO);
		} else if (mapUnitId.size() == 1) {
			createUnitPage();
		} else {
			Screen::setMainWidget(mainLayoutUnitChoice);
			activeMainLayout = mainLayoutUnitChoice;
		}
	} else if (listView == lVUnit) {
		String unitTmp = "operateur : ";
		for (int idx = 0; idx < mapLVIUnit.size(); idx++) {
			if (mapLVIUnit[idx] == listViewItem) {
				selectedUnit = idx;
				unitTmp += mapLUnitName[idx]->getText();
			}
		}
		unit->setText(unitTmp);
		Screen::setMainWidget(mainLayoutOptionChoice);
		activeMainLayout = mainLayoutOptionChoice;
	} else if (listView == lVUser) {
		for (int idx = 0; idx < mapLVIUser.size(); idx++) {
			if (mapLVIUser[idx] == listViewItem) {
				selectedUser = idx;
			}
		}
		String urlTmp1 = HOST;
		urlTmp1 += "/organizations/users/";
		urlTmp1 += Convert::toString(mapUserId[selectedUser]);
		urlTmp1 += "/medias/";
		urlTmp1 += _LOGINTOKEN;
		connectUrl(urlTmp1, MEDIA_TYPE);
	} else if (listView == lVMedia) {
		for (int idx = 0; idx < mapLVIMedia.size(); idx++) {
			if (mapLVIMedia[idx] == listViewItem) {
				selectedMedia = idx;
			}
		}
		String urlTmp1 = HOST;
		urlTmp1 += "/organizations/users/";
		urlTmp1 += Convert::toString(mapUserId[selectedUser]);
		urlTmp1 += "/medias/";
		urlTmp1 += Convert::toString(mapMediaId[selectedMedia]);
		urlTmp1 += _LOGINTOKEN;
		connectUrl(urlTmp1, MEDIA_VALUE_LIST);
	} else if (listView == lVMediaValue) {
		for (int idx = 0; idx < mapLVIMediaValue.size(); idx++) {
			if (mapLVIMediaValue[idx] == listViewItem) {
				selectedMediaValue = idx;
			}
		}
		createSnoozePage();
	}
}

void AlertTab::buttonClicked(Widget* button) {
	lprintfln("bouton click");
	if (button == bAddAlert) {
		String urlTmp = HOST;
		urlTmp += "/assets/";
		urlTmp += _LOGINTOKEN;
		connectUrl(urlTmp, ASSET_LIST);
	} else if (button == bCancel) {
		mapLVIAlert[posOptionAlert]->removeChild(HLOptionAlert);
		mapLVIAlert[posOptionAlert]->addChild(mapLAlertName[posOptionAlert]);
		posOptionAlert = -1;
	} else if (button == bView) {
		//		TODO :
		String urlTmp = HOST;
		urlTmp += "/alerts/" + Convert::toString(mapAlertId[posOptionAlert]);
		urlTmp += _LOGINTOKEN;
		connectUrl(urlTmp, ALERT_INFO);
	} else if (button == bDelete) {
		//  Attention on ne peut pas regarder le resultat de la requete avec HTTP_DELETE
		HttpConnection httpCon(this);
		String urlTmp = HOST;
		urlTmp += "/alerts/" + Convert::toString(mapAlertId[posOptionAlert])
				+ _LOGINTOKEN;
//		httpCon.create(urlTmp.c_str(), HTTP_DELETE);
		int res = maHttpCreate(urlTmp.c_str(), HTTP_DELETE);
		if (res < 0) {
			lprintfln("unable to connect - %i\n", res);
			maMessageBox("ERREUR", "Impossible de se connecter");
		} else {
			maHttpFinish(res);
			String urlTmp = HOST;
			urlTmp += "/alerts/";
			urlTmp += _LOGINTOKEN;
			connectUrl(urlTmp, ALERT_LIST);
			for (int idx0 = 0; idx0 < mapAlertId.size(); idx0++) {
				lValert->removeChild(mapLVIAlert[idx0]);
			}
			mapAlertId.clear();
		}

	} else if (button == bDest) {
		maWidgetSetProperty(eBKeyValue->getWidgetHandle(),
										MAW_EDIT_BOX_SHOW_KEYBOARD, "false");
		maWidgetSetProperty(eBValue->getWidgetHandle(),
												MAW_EDIT_BOX_SHOW_KEYBOARD, "false");
		if (optionPageValid()) {

			String urlTmp = HOST;
			urlTmp += "/organizations/users/";
			urlTmp += _LOGINTOKEN;
			connectUrl(urlTmp, USER_LIST);
		}
	} else if (button == bAddToList) {
		maWidgetSetProperty(eBSnooze->getWidgetHandle(),
								MAW_EDIT_BOX_SHOW_KEYBOARD, "false");
		if (snoozePageValid()) {
			int index = mapSnoozeList.size();
			mapSnoozeList[index] = Convert::toInt(eBSnooze->getText().c_str());
			lprintfln("YOOOOO : %d", mapSnoozeList[index]);
			Screen::setMainWidget(mainLayoutListDestChoice);
			activeMainLayout = mainLayoutListDestChoice;
			mapMediaValueListId[index] = mapMediaValueId[selectedMediaValue];
			mapLListDestName[index] = new Label();
			mapLListDestName[index]->setText(
					mapLUserName[selectedUser]->getText());
			mapLVIListDest[index] = new ListViewItem();
			mapLVIListDest[index]->addChild(mapLListDestName[index]);
			lVListDest->addChild(mapLVIListDest[index]);
		}
	} else if (button == bAddOtherDest) {
		Screen::setMainWidget(mainLayoutUserChoice);
		activeMainLayout = mainLayoutUserChoice;
	} else if (button == bCreateAlert) {
		mapAMSId.clear();
		currentAMS = 0;
		String urlTmp = HOST;
		urlTmp += "/medias/specializations/";
		urlTmp += _LOGINTOKEN;
		String message = "{\"ams_snooze\": ";
		message += Convert::toString(mapSnoozeList[0]);
		message += ",\"mev_id\" : ";
		message += Convert::toString(mapMediaValueListId[0]);
		message += "}";
		connectUrl(urlTmp, POST_MEDIA_SPE, POST, message);
	}
}

void AlertTab::createDestListPage() {
	MAExtent screenSize = maGetScrSize();
	int width = EXTENT_X(screenSize);

	mainLayoutListDestChoice = new VerticalLayout();
	mainLayoutListDestChoice->fillSpaceHorizontally();
	mainLayoutListDestChoice->fillSpaceVertically();
	listDestTitle = new Label(Convert::tr(alert_create_Label_list_dest_choice + LANGUAGE));
	listDestTitle->fillSpaceHorizontally();
	mainLayoutListDestChoice->addChild(listDestTitle);

	lVListDest = new ListView();
	lVListDest->addListViewListener(this);
	lVListDest->fillSpaceVertically();
	mainLayoutListDestChoice->addChild(lVListDest);

	hLButtonListDest = new HorizontalLayout();
	lVIButtonListDest = new ListViewItem();

	bAddOtherDest = new Button();
	bAddOtherDest->setWidth(width / 2);
	bAddOtherDest->setText(
			Convert::tr(alert_create_Button_Add_another_recipient + LANGUAGE));
	bAddOtherDest->addButtonListener(this);
	lVIButtonListDest->addChild(bAddOtherDest);
	bCreateAlert = new Button();
	bCreateAlert->setWidth(width / 2);
	bCreateAlert->setLeftPosition(width / 2);
	bCreateAlert->setText(
			Convert::tr(alert_create_Button_Create_alert + LANGUAGE));
	bCreateAlert->addButtonListener(this);
	lVIButtonListDest->addChild(bCreateAlert);
	mainLayoutListDestChoice->addChild(lVIButtonListDest);

//	Screen::setMainWidget(mainLayoutListDestChoice); //DEBUG
}

void AlertTab::createSnoozePage() {
	mainLayoutSnoozeChoice = new VerticalLayout();
	mainLayoutSnoozeChoice->fillSpaceHorizontally();
	mainLayoutSnoozeChoice->fillSpaceVertically();

	snoozeTitle = new Label(Convert::tr(alert_create_Label_snooze_choice + LANGUAGE));
	snoozeTitle->fillSpaceHorizontally();
	mainLayoutSnoozeChoice->addChild(snoozeTitle);

	lSnooze = new Label();
	lSnooze->setText(Convert::tr(alert_create_Label_Snooze + LANGUAGE));
	mainLayoutSnoozeChoice->addChild(lSnooze);
	HorizontalLayout* hLSnooze = new HorizontalLayout();
	hLSnooze->fillSpaceVertically();
	eBSnooze = new EditBox();
//	eBSnooze->addEditBoxListener(this);
	eBSnooze->setInputMode(EDIT_BOX_INPUT_MODE_NUMERIC);
	hLSnooze->addChild(eBSnooze);
	lSecond = new Label();
	lSecond->setText(Convert::tr(alert_create_Label_Seconde + LANGUAGE));
	hLSnooze->addChild(lSecond);
	mainLayoutSnoozeChoice->addChild(hLSnooze);
	bAddToList = new Button();
	bAddToList->setText(Convert::tr(alert_create_Button_add_list + LANGUAGE));
	bAddToList->addButtonListener(this);
	mainLayoutSnoozeChoice->addChild(bAddToList);

	Screen::setMainWidget(mainLayoutSnoozeChoice);
	activeMainLayout = mainLayoutSnoozeChoice;
}

void AlertTab::createUnitPage() {
	if(mainLayoutUnitChoice == NULL)
	{
		mainLayoutUnitChoice = new VerticalLayout();
		mainLayoutUnitChoice->fillSpaceHorizontally();
		mainLayoutUnitChoice->fillSpaceVertically();
		unitTitle = new Label(Convert::tr(alert_create_Label_unit_choice + LANGUAGE));
		unitTitle->fillSpaceHorizontally();
		mainLayoutUnitChoice->addChild(unitTitle);
		lVUnit = new ListView();
		lVUnit->addListViewListener(this);
		mainLayoutUnitChoice->addChild(lVUnit);
	}
	for (int idx = 0; idx <= mapUnitId.size() - 1; idx++) {
		mapLVIUnit[idx] = new ListViewItem();
		mapLVIUnit[idx]->addChild(mapLUnitName[idx]);
		lVUnit->addChild(mapLVIUnit[idx]);
	}
	Screen::setMainWidget(mainLayoutUnitChoice);
	activeMainLayout = mainLayoutUnitChoice;
}

void AlertTab::createOptionPage() {
	mainLayoutOptionChoice = new VerticalLayout();
	mainLayoutOptionChoice->fillSpaceHorizontally();
	mainLayoutOptionChoice->fillSpaceVertically();


	optionTitle = new Label(Convert::tr(alert_create_Label_option_choice + LANGUAGE));
	optionTitle->fillSpaceHorizontally();
	mainLayoutOptionChoice->addChild(optionTitle);

	lVOption = new ListView();
	lVOption->addListViewListener(this);
	lVOption->fillSpaceVertically();

	HorizontalLayout* hlTmp = new HorizontalLayout();
	mainLayoutOptionChoice->addChild(hlTmp);
	keyValue = new Label();
	keyValue->setText(Convert::tr(alert_create_Label_key_value + LANGUAGE));

	eBKeyValue = new EditBox();
	hlTmp->addChild(keyValue);
	hlTmp->setHeight(50);
	hlTmp->addChild(eBKeyValue);

	HorizontalLayout *hLTmp1 = new HorizontalLayout();
	hLTmp1->setHeight(50);

	value = new Label();
	value->setText(Convert::tr(alert_create_Label_value + LANGUAGE));
	hLTmp1->addChild(value);
	eBValue = new EditBox();
	hLTmp1->addChild(eBValue);
	mainLayoutOptionChoice->addChild(hLTmp1);
	mapLVIOption[0] = new ListViewItem();
	unit = new Label();
	unit->setText("unit : ");
	mapLVIOption[0]->addChild(unit);
	lVOption->addChild(mapLVIOption[0]);
	mapLVIOption[1] = new ListViewItem();
		criterion = new Label();
		criterion->setText(Convert::tr(alert_create_Label_operator + LANGUAGE));
		mapLVIOption[1]->addChild(criterion);
		lVOption->addChild(mapLVIOption[1]);
	mainLayoutOptionChoice->addChild(lVOption);
	bDest = new Button();
	bDest->addButtonListener(this);
	bDest->setText(Convert::tr(alert_create_Button_Add_recipient + LANGUAGE));
	mainLayoutOptionChoice->addChild(bDest);
//	Screen::setMainWidget(mainLayoutOptionChoice);
//	activeMainLayout = mainLayoutOptionChoice;
}

void AlertTab::createDetailAlertPage() {
	mainLayoutAlertDetailChoice = new VerticalLayout();
	mainLayoutAlertDetailChoice->fillSpaceHorizontally();
	mainLayoutAlertDetailChoice->fillSpaceVertically();
	lAlertName = new Label();
	lAlertName->setTextHorizontalAlignment(MAW_ALIGNMENT_LEFT);
	mainLayoutAlertDetailChoice->addChild(lAlertName);
	lAlertOperator = new Label();
	mainLayoutAlertDetailChoice->addChild(lAlertOperator);
	lAlertValue = new Label();
	mainLayoutAlertDetailChoice->addChild(lAlertValue);
	lAlertUnit = new Label();
	mainLayoutAlertDetailChoice->addChild(lAlertUnit);
	lAlertKeyValue = new Label();
	mainLayoutAlertDetailChoice->addChild(lAlertKeyValue);
	lAlertMedia = new Label();
	mainLayoutAlertDetailChoice->addChild(lAlertMedia);
	lAlertSnooze = new Label();
	mainLayoutAlertDetailChoice->addChild(lAlertSnooze);
	lAlertRecipient = new Label("Medias for this alert : ");
	mainLayoutAlertDetailChoice->addChild(lAlertRecipient);
	lVRecipients = new ListView();
	lVRecipients->fillSpaceVertically();
	mainLayoutAlertDetailChoice->addChild(lVRecipients);

}

void AlertTab::createUI() {

	String urlTmp = HOST;
	urlTmp += "/alerts/";
	urlTmp += _LOGINTOKEN;
	connectUrl(urlTmp, ALERT_LIST);

	mainLayoutAlertChoice = new VerticalLayout();
// Make the layout fill the entire screen.
	mainLayoutAlertChoice->fillSpaceHorizontally();
	mainLayoutAlertChoice->fillSpaceVertically();
	lListTitle = new Label(Convert::tr(alert_list_Label_title + LANGUAGE));
	lListTitle->fillSpaceHorizontally();
	mainLayoutAlertChoice->addChild(lListTitle);
	Screen::setMainWidget(mainLayoutAlertChoice);
	activeMainLayout = mainLayoutAlertChoice;
	lValert = new ListView();
	lValert->addListViewListener(this);
	lValert->fillSpaceVertically();
	ListViewItem *lviAddAlert = new ListViewItem();
	bAddAlert = new Button();
	bAddAlert->addButtonListener(this);
	bAddAlert->fillSpaceHorizontally();
//	bAddAlert->setText("Alert.button-add-new-alert");
	bAddAlert->setText(Convert::tr(alert_create_Button_add_alert + LANGUAGE));
	lviAddAlert->addChild(bAddAlert);
	mainLayoutAlertChoice->addChild(lValert);
	mainLayoutAlertChoice->addChild(lviAddAlert);
	createDestListPage();
	createDetailAlertPage();
}

bool AlertTab::optionPageValid() {
	if (eBKeyValue->getText() == "") {
		maMessageBox("Warning", "Key value can not be empty");
		return false;
	} else if (selectedOperator == -1) {
		maMessageBox("Warning", "Operator is not selected");
		return false;
	} else if (eBValue->getText() == "") {
		maMessageBox("Warning", "Value can not be empty");
		return false;
	}
	return true;
}

bool AlertTab::snoozePageValid() {
	if (eBSnooze->getText() == "") {
		maMessageBox("Warning", "Snooze can not be empty");
		return false;
	} else if (Convert::toInt(eBSnooze->getText().c_str()) < 60) {
		maMessageBox("Warning", "Snooze should be higher than 60");
		return false;
	}
	return true;
}

//void AlertTab::drawChangeVerticalHorizontal(int width, int height) {
//
//	if (width < height) // Landscape
//			{
//		setTitle(Convert::getString(CREATE_ALERT_TAB_EN));
//	} else // Portrait
//	{
//		setTitle("");
//		setIcon(LOGO);
//	}
//}
