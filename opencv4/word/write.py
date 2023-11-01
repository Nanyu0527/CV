import numpy as np
import tensorflow as tf
from tensorflow.examples.tutorials.mnist import input_data


mnist = input_data.read_data_sets('mnist_data',one_hot=True)
input_x = tf.placeholder(tf.float32,[None,28*28])/255 
output_y = tf.placeholder(tf.int32,[None,10])
input_x_images = tf.reshape(input_x,[-1,28,28,1])
test_x = mnist.test.images[:3000]
test_y = mnist.test.labels[:3000]
 
conv1 = tf.layers.conv2d(
inputs=input_x_images,
filters=32,
kernel_size=[5,5],
strides=1,
padding='same',
activation=tf.nn.relu
)

pool1 = tf.layers.max_pooling2d(
    inputs=conv1,
    pool_size=[2,2],
    strides=2
)#缩小一半

conv2 = tf.layers.conv2d(
inputs=pool1,
filters=64,
kernel_size=[5,5],
strides=1,
padding='same',
activation=tf.nn.relu
)

pool2 = tf.layers.max_pooling2d(
    inputs=conv2,
    pool_size=[2,2],
    strides=2
)

#压平
flat = tf.reshape(pool2,[-1,7*7*64])
#全连接层
dense=tf.layers.dense(
    inputs=flat,
    units=1024,
    activation=tf.nn.relu
)
#丢弃冗余的网络节点
dropout = tf.layers.dropout(
    inputs=dense,
    rate=0.5
)
#输出
outputs = tf.layers.dense(
    inputs=dropout,
    units=10
)

loss = tf.losses.softmax_cross_entropy(onehot_labels=output_y,logits=outputs)
train_op = tf.train.GradientDescentOptimizer(0.001).minimize(loss)

accuracy_op = tf.metrics.accuracy(
    labels=tf.argmax(output_y,axis=1), 
    predictions=tf.argmax(outputs,axis=1)
)
 
sess=tf.Session()
init=tf.group(tf.global_variables_initializer(),tf.local_variables_initializer())
sess.run(init)
#训练了2000个批次
for i in range(2000):
    batch = mnist.train.next_batch(50)
    train_loss, train_op_=sess.run([loss,train_op],{input_x:batch[0],output_y:batch[1]})
    if i%100==0:
        test_accuracy=sess.run(accuracy_op,{input_x:test_x,output_y:test_y})
        print("Step=%d, Train loss=%.4f,Test accuracy=%.2f"%(i,train_loss,test_accuracy[0]))#输出当前步数，训练损失，准确率
  
test_output=sess.run(outputs,{input_x:test_x[:30]})
inferenced_y=np.argmax(test_output,1)
print(inferenced_y,'Inferenced numbers')
print(np.argmax(test_y[:30],1),'Real numbers') 
sess.close()
