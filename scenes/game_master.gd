extends Node
class_name GameMaster

var enemy: PackedScene = preload("res://scenes/enemy.tscn")

var all_enemies : Array[CharacterBody3D]

func _ready():
	var newEnemy: CharacterBody3D = enemy.instantiate()
	self.add_child(newEnemy)
	newEnemy.global_position = Vector3(-10.24, 1.612, 0)
	all_enemies.append(newEnemy)
