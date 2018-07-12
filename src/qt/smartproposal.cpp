#include "smartproposal.h"
#include "ui_smartproposal.h"
#include "amount.h"
#include "bitcoinunits.h"
#include "math.h"

#include <QDesktopServices>

SmartProposalWidget::SmartProposalWidget(SmartProposal * proposal, QWidget *parent) :
    QFrame(parent),
    ui(new Ui::SmartProposalWidget)
{
    ui->setupUi(this);

    ui->votedTypeLabel->setHidden(false);
    ui->votedAmountLabel->setHidden(true);

    voteSelection.addButton(ui->disabledButton, 0);
    voteSelection.addButton(ui->yesButton, 1);
    voteSelection.addButton(ui->noButton, 2);
    voteSelection.addButton(ui->abstainButton, 3);

    this->proposal = *proposal;

    QDateTime deadline = QDateTime::fromString(proposal->getVotingDeadline(), Qt::ISODate);
    deadline.setTimeSpec(Qt::UTC);

    QDateTime created = QDateTime::fromString(proposal->getCreatedDate(), Qt::ISODate);
    deadline.setTimeSpec(Qt::UTC);

    QDateTime current = QDateTime::currentDateTimeUtc();

    time_t deadlineSec = deadline.toTime_t();
    time_t createdSec = created.toTime_t();
    time_t currentSec = current.toTime_t();
    time_t totalSec = deadlineSec - createdSec;
    time_t remainingSec = deadlineSec - currentSec;
    int votingProgress = 100 - (double(remainingSec) / double(totalSec) * 100);

    ui->deadlineLabel->setText(deadline.toString(Qt::SystemLocaleLongDate));
    ui->deadlineProgress->setValue(votingProgress);

    QString proposalTitle = QString("#%1 - %2").arg(proposal->getProposalId()).arg(proposal->getTitle());

    ui->titleLabel->setText(proposalTitle);

    CAmount yes = proposal->getVoteYes() * COIN;
    CAmount no = proposal->getVoteNo() * COIN;
    CAmount abstain = proposal->getVoteAbstain() * COIN;

    int nDisplayUnit = BitcoinUnits::SMART;

    QString smartString = QString::number((int)proposal->getAmountSmart());
    QString usdString = QString::number((int)proposal->getAmountUSD());

    QChar thin_sp(THIN_SP_CP);

    int q_size = smartString.size();
    for (int i = 3; i < q_size; i += 3)
        smartString.insert(q_size - i, thin_sp);

    q_size = usdString.size();
    for (int i = 3; i < q_size; i += 3)
        usdString.insert(q_size - i, thin_sp);

    ui->amountSmartLabel->setText(QString("%1 USD").arg(usdString));
    ui->amountUSDLabel->setText(QString("%1 SMART").arg(smartString));

    ui->yesLabel->setText(QString("Yes %1\% ( %2 SMART )").arg(QString::number(proposal->getPercentYes(),'f',2)).arg(BitcoinUnits::format(nDisplayUnit, yes)));
    ui->noLabel->setText(QString("No %1\% ( %2 SMART )").arg(QString::number(proposal->getPercentNo(),'f',2)).arg(BitcoinUnits::format(nDisplayUnit, no)));
    ui->abstainLabel->setText(QString("Abstain %1\% ( %2 SMART )").arg(QString::number(proposal->getPercentAbstain(),'f',2)).arg(BitcoinUnits::format(nDisplayUnit, abstain)));

    ui->progressYes->setValue( (int)proposal->getPercentYes() );
    ui->progressNo->setValue( (int)proposal->getPercentNo() );
    ui->progressAbstain->setValue( (int)proposal->getPercentAbstain() );

    connect(ui->viewProposalButton, SIGNAL(clicked()),this, SLOT(viewProposal()));
    connect(ui->disabledButton, SIGNAL(clicked()), this, SLOT(voteButtonClicked()));
    connect(ui->yesButton, SIGNAL(clicked()), this, SLOT(voteButtonClicked()));
    connect(ui->noButton, SIGNAL(clicked()), this, SLOT(voteButtonClicked()));
    connect(ui->abstainButton, SIGNAL(clicked()), this, SLOT(voteButtonClicked()));
}

void SmartProposalWidget::viewProposal(){
    QDesktopServices::openUrl(QUrl("https://vote.smartcash.cc/Proposal/Details/" + proposal.getUrl()));
}

void SmartProposalWidget::voteButtonClicked(){
    voteChanged();
}

SmartProposalWidget::~SmartProposalWidget()
{
    delete ui;
}

SmartHiveVoting::Type SmartProposalWidget::getVoteType()
{

    if( voteSelection.checkedId() == 1) return SmartHiveVoting::Yes;
    if( voteSelection.checkedId() == 2) return SmartHiveVoting::No;
    if( voteSelection.checkedId() == 3) return SmartHiveVoting::Abstain;

    return SmartHiveVoting::Disabled;
}

void SmartProposalWidget::setVoted(const SmartProposalVote &vote)
{

    if( vote.GetVotingPower() ){
        ui->votedTypeLabel->setHidden(false);
        ui->votedAmountLabel->setHidden(false);
        ui->votedTypeLabel->setText(QString::fromStdString(vote.GetVoteType()));

        QString votedText = QString::number((int)(vote.GetVotingPower() / COIN));

        QChar thin_sp(THIN_SP_CP);

        int q_size = votedText.size();
        for (int i = 3; i < q_size; i += 3)
            votedText.insert(q_size - i, thin_sp);

        ui->votedAmountLabel->setText(votedText + QString(" SMART"));
    }

}
