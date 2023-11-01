import torch
import torchvision
import torchvision.transforms as transforms
import torch.nn as nn
import torch.optim as optim
import matplotlib.pyplot as plt
import numpy as np

batch_size = 16
transform = transforms.Compose(
    [transforms.ToTensor(), 
     transforms.Normalize((0.5, 0.5, 0.5), (0.5, 0.5, 0.5))])

# CIFAR10总共10个类别
classes = ('plane', 'car', 'bird', 'cat',
           'deer', 'dog', 'frog', 'horse', 'ship', 'truck')

class Net(nn.Module):
    def __init__(self):
        super().__init__()
        self.classifier = nn.Sequential(
            nn.Conv2d(3, 6, 5),
            nn.ReLU(),
            nn.MaxPool2d(2, 2),
            nn.Conv2d(6, 16, 5),
            nn.ReLU(),
            nn.MaxPool2d(2, 2),
            nn.Flatten(),
            nn.Linear(16 * 5 * 5, 120),
            nn.ReLU(),
            nn.Linear(120, 84),
            nn.ReLU(),
            nn.Linear(84, 10)
        )

    def forward(self, x):
        return self.classifier(x)

net = Net()
Loss = nn.CrossEntropyLoss()#损失函数
optimizer = optim.SGD(net.parameters(), lr=0.001, momentum=0.9)#优化器

trainset = torchvision.datasets.CIFAR10(root='./data', train=True,
                                        download=True, transform=transform)
trainloader = torch.utils.data.DataLoader(trainset, batch_size=batch_size,
                                          shuffle=True)
testset = torchvision.datasets.CIFAR10(root='./data', train=False,
                                       download=True, transform=transform)
testloader = torch.utils.data.DataLoader(testset, batch_size=batch_size,
                                         shuffle=False)


nums = 20 
print("test if gpu is avaliable:")
print(torch.cuda.is_available())
print("Start training...")
for num in range(nums):#20次迭代遍历
    running_loss = 0.0
    for i, data in enumerate(trainloader):
        inputs, labels = data
        optimizer.zero_grad()
        outputs = net(inputs)
        loss = Loss(outputs, labels)
        loss = loss.cuda()
        loss.backward()
        optimizer.step()
        running_loss += loss.item()
        if i % 2000 == 1999:   #每2000次打印一次损失
            print(f'[{num + 1}, {i + 1:5d}] loss: {running_loss / 2000:.3f}')
            running_loss = 0.0

print("Finish training")
correct = 0
total = 0
with torch.no_grad():
    for data in testloader:
        images, labels = data
        outputs = net(images)
        _, predicted = torch.max(outputs, 1)
        total += labels.size(0)
        correct += (predicted == labels).sum().item()
print(f'Accuracy: {100 * correct // total} %')

# 统计每个类别的正确率
cor = {classname: 0 for classname in classes}
tot = {classname: 0 for classname in classes}

with torch.no_grad():
    for data in testloader:
        images, labels = data
        outputs = net(images)
        _, predictions = torch.max(outputs, 1)#每行最大值
        for label, prediction in zip(labels, predictions):
            if label == prediction:
                cor[classes[label]] += 1
            tot[classes[label]] += 1

for classname, proper in cor.items():
    accuracy = 100 * float(proper) / tot[classname]
    print(f'Accuracy for class: {classname:5s} is {accuracy:.1f} %')
