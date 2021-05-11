#include "remote.h"
#include "actor.h"


Remote::Remote(QWidget *parent) : QWidget(parent){
    setWindowTitle("Remote Access");
//    resize(300, 300);

    mainLayout = new QVBoxLayout(this);
//    setLayout(mainLayout);

    fpsDisplay = new QLabel("X FPS", this);
//    fpsDisplay->move(10, 10);
//    fpsDisplay->resize(100, 10)
    mainLayout->addWidget(fpsDisplay);

    colWireToggle = new QCheckBox("Enable collision wireframes", this);
    colWireToggle->setChecked(false);
//    colWireToggle->move(10, 40);
    mainLayout->addWidget(colWireToggle);
    modelWireToggle = new QCheckBox("Enable model wireframes", this);
    modelWireToggle->setChecked(false);
    mainLayout->addWidget(modelWireToggle);
//    modelWireToggle->move(10, 70);

    // Following widgets are used for modifying actor data during gameplay
    // Combobox for selecting an actor
    actorSelect = new QComboBox(this);
    connect(actorSelect, SIGNAL(activated(int)), this, SLOT(remoteActorSelected(int)));
    mainLayout->addWidget(actorSelect);

    // FormLayout to hold field labels and editable fields
    actorInfoLayout = new QFormLayout(this);
    mainLayout->addLayout(actorInfoLayout);

    // Actor name
    actorName = new QLineEdit();
    actorInfoLayout->addRow("Name", actorName);

    // Model and texture change section
    // Unlike other fields, these fields are changed separately (instead of using "Apply all")
    // Model select
    modelSelectContainer = new QHBoxLayout();
    modelSelectLine = new QLineEdit();
    modelSelectBrowse = new QPushButton("Browse");
    modelSelectApply = new QPushButton("Apply");
    connect(modelSelectBrowse, SIGNAL(clicked(bool)), this, SLOT(browseModel()));
    connect(modelSelectApply, SIGNAL(clicked(bool)), this, SLOT(updateStoredModel()));
    modelSelectContainer->addWidget(modelSelectLine);
    modelSelectContainer->addWidget(modelSelectBrowse);
    modelSelectContainer->addWidget(modelSelectApply);
    actorInfoLayout->addRow("Change model", modelSelectContainer);
    // Texture select
    textureSelectContainer = new QHBoxLayout();
    textureSelectLine = new QLineEdit();
    textureSelectBrowse = new QPushButton("Browse");
    textureSelectApply = new QPushButton("Apply");
    connect(textureSelectBrowse, SIGNAL(clicked(bool)), this, SLOT(browseTexture()));
    textureSelectContainer->addWidget(textureSelectLine);
    textureSelectContainer->addWidget(textureSelectBrowse);
    textureSelectContainer->addWidget(textureSelectApply);
    actorInfoLayout->addRow("Change texture", textureSelectContainer);
    connect(textureSelectApply, SIGNAL(clicked(bool)), this, SLOT(updateStoredTexture()));

    // Actor visibility, collision, and logic toggle
    actorModelToggle = new QCheckBox();
    actorCollisionToggle = new QCheckBox();
    actorLogicToggle = new QCheckBox();
    actorLogicToggle->setDisabled(true);
    actorInfoLayout->addRow("Toggle model", actorModelToggle);
    actorInfoLayout->addRow("Toggle collision", actorCollisionToggle);
    actorInfoLayout->addRow("Toggle logic", actorLogicToggle);

    // Actor position
    actorPositionLayout = new QHBoxLayout();
    posX = new QDoubleSpinBox();
    posX->setRange(-100000000,100000000);
    posY = new QDoubleSpinBox();
    posY->setRange(-100000000,100000000);
    posZ = new QDoubleSpinBox();
    posZ->setRange(-100000000,100000000);
    actorPositionLayout->addWidget(posX);
    actorPositionLayout->addWidget(posY);
    actorPositionLayout->addWidget(posZ);
    actorInfoLayout->addRow("Position", actorPositionLayout);

    // Actor velocity

    // Actor gravity
    actorGravity = new QDoubleSpinBox();
    actorGravity->setRange(-10.0, 10.0);
    actorInfoLayout->addRow("Gravity", actorGravity);

    // Button that applies all current values in the remote to the selected actor
    // If this is not pressed and MainWindow regains focus, most changes will be lost
    applyButton = new QPushButton("Apply changes (returning to MainWindow will discard unapplied changes)");
    connect(applyButton, SIGNAL(clicked(bool)), this, SLOT(updateStoredActor()));
    mainLayout->addWidget(applyButton);

    // Button that opens a widget for setting a new AABB
    // You can enter it's size manually or pass an .obj that will be converted to an AABB

    // Button that creates a new, empty Actor or ActorDynamic and changes the ComboBox to this new actor
    // It's texture, model etc. can be set using the remote
    // HERE PUT: NEW ACTOR, NEW DYNAMIC ACTOR, EXPORT ACTOR, LOAD ACTOR(from file)
}

void Remote::start(){
    active = true;
    raise();
    activateWindow();

    show();
    qDebug("Remote started");
}

void Remote::trackDeltas(float delta){
    // Track the deltas over the last second and display average fps
    // This assumes that the target fps are 60
    deltaQueue.push_back(delta);
//    if(deltaQueue.size() > 60){
//        deltaQueue.pop_front();
//    }

    float sum = 0;
    for(float delta : deltaQueue){
        sum += delta;
    }

    if(deltaQueue.size() >= 60){
        float averageDelta = sum / (float)deltaQueue.size();
        int averageFps = 1.0/averageDelta;
        fpsDisplay->setText((std::to_string(averageFps) + " FPS").c_str());
        deltaQueue.clear();
    }
}

void Remote::updateActorSelect(std::vector<std::string> names){
    actorSelect->clear();
    for(auto name : names){
        actorSelect->insertItem(actorSelect->count(), name.c_str());
    }
}

void Remote::updateActorInfo(Actor *actor){
    storedActor = actor;

    actorName->setText(actor->name.c_str());

    posX->setValue(actor->position.x);
    posY->setValue(actor->position.y);
    posZ->setValue(actor->position.z);

    actorModelToggle->setChecked(actor->visible);
    actorCollisionToggle->setChecked(actor->collisionEnabled);

    actorLogicToggle->setDisabled(true);
    actorLogicToggle->setChecked(false);

//    actorGravity->setDisabled(true);
//    actorGravity->setValue(0.1f);
    actorGravity->setValue(actor->gravity);
}

void Remote::closeEvent(QCloseEvent *event){
    active = false;
    event->accept();
}

void Remote::browseModel(){
    QString filepath = QFileDialog::getOpenFileName(this,
                            tr("Select file"), QDir::currentPath(),
                            tr("Object files (*.obj)"));

    if(!filepath.isNull()){
        modelSelectLine->setText(filepath);
    }

}

void Remote::browseTexture(){
    QString filepath = QFileDialog::getOpenFileName(this,
                            tr("Select file"), QDir::currentPath(),
                            tr("Images (*.png *.jpg)"));

    if(!filepath.isNull()){
        textureSelectLine->setText(filepath);
    }
}

void Remote::updateStoredModel(){
    QString filepath = modelSelectLine->text();
    T3::MeshTexture newMesh;
    newMesh.texture = storedActor->getModel().texture;
    if(newMesh.loadFromFile(filepath)){
//      if(newMesh.loadFromFile(modelSelectLine->text()))
        qDebug("Model changed");
    }else{
        qDebug("Failed to change model");
        qDebug("%s", filepath);
    }

    if(!newMesh.empty()){
        storedActor->setModel(newMesh);
    }
}

void Remote::updateStoredModel(QString filepath){
    T3::MeshTexture newMesh;
    newMesh.texture = storedActor->getModel().texture;
    if(newMesh.loadFromFile(filepath)){
        qDebug("Model changed");
    }else{
        qDebug("Failed to change model");
    }

    storedActor->setModel(newMesh);
}

void Remote::updateStoredTexture(){
    QString filepath = textureSelectLine->text();

    storedActor->setTexture(filepath);
}

void Remote::updateStoredActor(){
    storedActor->visible = actorModelToggle->isChecked();
    storedActor->collisionEnabled = actorCollisionToggle->isChecked();
//    storedActor->logicEnabled = actorLogicToggle->isChecked();
    storedActor->position.x = posX->value();
    storedActor->position.y = posY->value();
    storedActor->position.z = posZ->value();
    storedActor->gravity = actorGravity->value();
}

//void Remote::updateStoredName(){

//}

//void Remote::updateStoredToggleModel(){

//}

void Remote::remoteActorSelected(int index){
    emit updateRemoteActor(index);
}

//void Remote::updateActorValues(){

//}

