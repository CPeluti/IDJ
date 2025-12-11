extends Node
class_name GameMaster

var enemy: PackedScene = preload("res://scenes/enemy.tscn")
var number_of_enemies: int = 5

static var all_enemies : Array[CharacterBody3D]

func getClosestEnemy(reference: Vector3, range: float, ignore: Array[CharacterBody3D] = []) -> CharacterBody3D:
	var minDistance: float = 50.0
	var closestEnemy: CharacterBody3D
	
	for e in all_enemies:
		var distance: float = (e.global_position - reference).length_squared()
		if ignore.find(e) >= 0:
			continue
		if (distance <= minDistance and distance <= range):
			minDistance = distance
			closestEnemy = e
	return closestEnemy
		

func _ready():
	for i in range(0, number_of_enemies):
		var newEnemy: CharacterBody3D = enemy.instantiate()
		self.add_child(newEnemy)
		newEnemy.global_position = Vector3(-10.24, 1.612, 0)
		newEnemy.scale = Vector3(2.37, 2.37, 2.37)
		all_enemies.append(newEnemy)
	
